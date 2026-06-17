#include "ParticleManager.h"
#include <MatrixVector.h>
#include <cassert>
#include <ModelManager.h>
#include <TextureManager.h>
#include <numbers>
#include <externals/DirectXTex/d3dx12.h>
#include <Object3dCommon.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include <ResourceFactory.h>
#include <CameraManager.h>
#include <ParticleGroupfactory.h>

using namespace Microsoft::WRL;

namespace MyEngine {

    using namespace MatrixVector;
    using namespace ResourceFactory;

    // 静的メンバ変数の定義
    std::unique_ptr<ParticleManager> ParticleManager::instance = nullptr;

    // シングルトンインスタンスの取得
    ParticleManager* ParticleManager::GetInstance() {
        if (!instance) {
            instance = std::make_unique<ParticleManager>();
        }
        return instance.get();
    }

    // 終了
    void ParticleManager::Finalize() {
        instance.reset();
    }

    void ParticleManager::ClearAll() {
        particleGroups.clear();   // 全てのパーティクルグループを削除
    }

    void ParticleManager::Initialize(DirectXCommon* birectxcommon, SrvManager* srvmanager, ParticleCommon* particleCommon) {
        // NULL検出
        assert(birectxcommon);
        assert(srvmanager);
        // メンバ変数に記録
        this->dxCommon_ = birectxcommon;
        this->srvmanager_ = srvmanager;
        this->particleCommon_ = particleCommon;
        // 乱数エンジンを初期化
        std::random_device rd;// 乱数生成器
        randomEngine = std::mt19937(rd());
        //ビルボード行列作成
        backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
        // 各種 GPU リソース・バッファの生成と初期化
        std::function<Microsoft::WRL::ComPtr<ID3D12Resource>(ID3D12Device*, size_t) > createBuffer = [this](ID3D12Device* device, size_t size) {
            return CreateBufferResource(device, size);            };
        cameraBuffer_.Create(dxCommon_->GetDevice().Get(), createBuffer);
        particleInfoBuffer_.Create(dxCommon_->GetDevice().Get(), createBuffer);
        spawnListBuffer_.Create(dxCommon_->GetDevice().Get(), createBuffer, MaxSpawnRequestCount);
        groupSpawnBuffer_.Create(dxCommon_->GetDevice().Get(), createBuffer, MaxGroupCount);
        attractInfoBuffer_.Create(dxCommon_->GetDevice().Get(), createBuffer, MaxGroupCount);
    }

    void ParticleManager::Update() {
        Matrix4x4 billboardMatrix;

        // カメラを CameraManager 経由で取得
        Camera* activeCamera = CameraManager::GetInstance()->GetActiveCamera();
        if (!activeCamera) { return; }

        // カメラ行列更新（GPUへ渡す）
        billboardMatrix = Multiply(backToFrontMatrix, activeCamera->GetWorldMatrix());
        cameraBuffer_->view = activeCamera->GetViewMatrix();
        cameraBuffer_->projection = activeCamera->GetProjectionMatrix();
        // パーティクルの位置をカメラの方向に合わせるために設定
        billboardMatrix.m[3][0] = 0.0f;
        billboardMatrix.m[3][1] = 0.0f;
        billboardMatrix.m[3][2] = 0.0f;
        cameraBuffer_->billboard = billboardMatrix;
        //---------------------------------
        // Attract情報更新
        //---------------------------------
        attractInfoBuffer_->targetPosition = attractTargetPosition_;
        attractInfoBuffer_->deltaTime = 1.0f / 60.0f;

        // DescriptorHeap設定
        srvmanager_->PreDraw();

        // リソースを一括でUAV状態へ遷移
        for (auto& [name, group] : particleGroups) {
            TransitionParticleBuffer(group, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        }

        // スポーンキューキューの蓄積・Dispatch処理
        std::vector<ActiveGroupSpawn> activeSpawns;
        uint32_t totalRequests = BuildSpawnRequests(activeSpawns);

        if (totalRequests > 0 && !activeSpawns.empty()) {
            // 前フェーズとの衝突を防ぐため、対象リソースのUAVバリアを一括適用
            std::vector<ID3D12Resource*> targetResources;
            for (const auto& spawn : activeSpawns) {
                targetResources.push_back(spawn.group->Resource.Get());
                targetResources.push_back(spawn.group->freeListResource.Get());
                targetResources.push_back(spawn.group->freeCounterResource.Get());
            }
            PipelineUAVBarriers(targetResources);
            // スポーン処理のDispatch
            DispatchSpawnCommands(activeSpawns);
            // スポーン完了後に次のUpdate（シミュレーション）へ繋ぐバリア
            PipelineUAVBarriers(targetResources);
        }

        // パーティクルの動き・寿命等のシミュレーション更新 (Update CS)
        particleCommon_->CommandUpdate();
        std::vector<ID3D12Resource*> updateResources;

        for (auto& [name, group] : particleGroups) {
            particleInfoBuffer_->particleCount = group.maxInstanceCount;
            // root0 : particle
            srvmanager_->SetComputeRootDescriptorTable(0, group.uavIndex);
            // root1 : freeList
            srvmanager_->SetComputeRootDescriptorTable(1, group.freeListUavIndex);
            // root2 : freeCounter
            srvmanager_->SetComputeRootDescriptorTable(2, group.freeCounterUavIndex);
            // root3 : ParticleInfo
            dxCommon_->GetCommandList()->SetComputeRootConstantBufferView(3, particleInfoBuffer_.GetGPUVirtualAddress());
            // root4 : AttractInfo
            dxCommon_->GetCommandList()->SetComputeRootConstantBufferView(4, attractInfoBuffer_.GetGPUVirtualAddress());
            int32_t threadGroupCount = (group.maxInstanceCount + 255) / 256;
            dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);
            updateResources.push_back(group.Resource.Get());
            updateResources.push_back(group.freeListResource.Get());
            updateResources.push_back(group.freeCounterResource.Get());
        }

        if (!updateResources.empty()) {
            PipelineUAVBarriers(updateResources);
        }
    }

    void ParticleManager::Draw() {
        // パーティクルグループごとに描画処理を行う
        for (auto& [name, particleGroup] : particleGroups) {
            // UAV -> SRV
            TransitionToDrawState(particleGroup);
            // インスタンシングデータの SRV を設定（テクスチャファイルのパスを指定）
            srvmanager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvindex);
            // SRVで画像を表示
            srvmanager_->SetGraphicsRootDescriptorTable(2, particleGroup.materialData.textureindex);
            // カメラ用定数バッファをセット
            dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraBuffer_.GetGPUVirtualAddress());
            // モデルに必要なバッファをバインド（頂点バッファや定数バッファなど）
            particleGroup.model->Draw();
            // 描画（インスタンシング）を実行
            dxCommon_->GetCommandList()->DrawInstanced(static_cast<UINT>(particleGroup.model->GetVertexCount()), static_cast<UINT>(particleGroup.maxInstanceCount), 0, 0);
        }
    }

    void ParticleManager::CreateParticleGroup(const std::string& name, const std::string& textureFilepath, const std::string& filename, const uint32_t& MaxInstanceCount) {
        // 1. まずファクトリーを介してテクスチャをロード＆既存チェック（自分自身のマップのアドレスを渡す）
        // すでに存在するか、テクスチャの更新が終わっていればここで return される
        if (ParticleGroupfactory::GetInstance()->Initialize(name, textureFilepath, &particleGroups)) {
            return;
        }

        // 2. 既存になかった場合のみ、新規グループをファクトリーに丸投げして新規グループを追加
        particleGroups[name] = ParticleGroupfactory::GetInstance()->CreateNewGroup(dxCommon_, srvmanager_, filename, textureFilepath, MaxInstanceCount);
    }

    void ParticleManager::Emit(const std::string& name, const ParticleSpawnData& spawnData) {
        spawnRequests_.push_back({ name,spawnData });
    }

    uint32_t ParticleManager::BuildSpawnRequests(std::vector<ActiveGroupSpawn>& outActiveSpawns) {
        if (spawnRequests_.empty()) { return 0; }

        uint32_t totalRequestCount = 0;

        for (const auto& request : spawnRequests_) {
            auto it = particleGroups.find(request.groupName);
            if (it == particleGroups.end()) { continue; }
            if (outActiveSpawns.size() >= MaxGroupCount) { break; }

            ParticleGroup& group = it->second;
            const auto& spawnData = request.spawnData;
            uint32_t groupStartIndex = totalRequestCount;

            for (uint32_t i = 0; i < spawnData.count; ++i) {
                if (totalRequestCount >= MaxSpawnRequestCount) { break; }

                SpawnRequestGPU& dst = spawnListBuffer_.data[totalRequestCount];
                dst.translate = spawnData.transform.translate;
                dst.rotate = spawnData.transform.rotate;
                dst.scale = spawnData.transform.scale;

                dst.color = spawnData.color;

                dst.velocityTranslate = spawnData.velocity.translate;
                dst.velocityRotate = spawnData.velocity.rotate;
                dst.velocityScale = spawnData.velocity.scale;

                dst.lifetime = spawnData.lifetime;
                dst.useGravity = spawnData.useGravity ? 1u : 0u;
                //-------------------------
                // behavior
                //-------------------------
                dst.behaviorType = static_cast<uint32_t>(spawnData.behavior);
                // stopTime
                dst.stopTime = spawnData.stopTime;
                // target
                dst.targetPosition = spawnData.targetPosition;

                totalRequestCount++;
            }

            uint32_t groupSpawnCount = totalRequestCount - groupStartIndex;
            if (groupSpawnCount > 0) {
                outActiveSpawns.push_back({ &group, groupStartIndex, groupSpawnCount });
            }
        }

        spawnRequests_.clear();
        return totalRequestCount;
    }

    void ParticleManager::DispatchSpawnCommands(const std::vector<ActiveGroupSpawn>& activeSpawns) {
        particleCommon_->CommandSpawn();
        uint32_t cbCBVIndex = 0;

        for (const auto& spawn : activeSpawns) {
            if (cbCBVIndex >= MaxGroupCount) { break; }

            groupSpawnBuffer_.GetData()[cbCBVIndex].startRequestIndex = spawn.startIndex;
            groupSpawnBuffer_.GetData()[cbCBVIndex].spawnCount = spawn.count;
            //---------------------------------
            // RootParameter Bind            
            //---------------------------------
            // root0 : u0 particle
            srvmanager_->SetComputeRootDescriptorTable(0, spawn.group->uavIndex);
            // root1 : u1 freeList
            srvmanager_->SetComputeRootDescriptorTable(1, spawn.group->freeListUavIndex);
            // root2 : u2 freeCounter
            srvmanager_->SetComputeRootDescriptorTable(2, spawn.group->freeCounterUavIndex);
            //---------------------------------
            // root3 : b0 GroupSpawnCB            
            //---------------------------------
            D3D12_GPU_VIRTUAL_ADDRESS cbvAddress = groupSpawnBuffer_.GetGPUVirtualAddress() + (cbCBVIndex * sizeof(GroupSpawnCB));
            dxCommon_->GetCommandList()->SetComputeRootConstantBufferView(3, cbvAddress);
            //---------------------------------
            // root4 : t0 SpawnRequest
            //---------------------------------            
            dxCommon_->GetCommandList()->SetComputeRootShaderResourceView(4, spawnListBuffer_.GetGPUVirtualAddress());
            //---------------------------------
            // Dispatch
            //---------------------------------
            uint32_t threadGroupCount = (spawn.count + 63) / 64;
            dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);

            cbCBVIndex++;
        }
    }

    void ParticleManager::TransitionParticleBuffer(ParticleGroup& group, D3D12_RESOURCE_STATES after) {
        std::vector<D3D12_RESOURCE_BARRIER> barriers;
        //---------------------------------
        // Particle Buffer
        //---------------------------------
        if (group.particleState != after) {
            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = group.Resource.Get();
            barrier.Transition.StateBefore = group.particleState;
            barrier.Transition.StateAfter = after;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barriers.push_back(barrier);
            group.particleState = after;
        }
        //---------------------------------
        // FreeList
        //---------------------------------
        if (group.freeListState != after) {
            D3D12_RESOURCE_BARRIER freelistbarrier{};
            freelistbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            freelistbarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            freelistbarrier.Transition.pResource = group.freeListResource.Get();
            freelistbarrier.Transition.StateBefore = group.freeListState;
            freelistbarrier.Transition.StateAfter = after;
            freelistbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barriers.push_back(freelistbarrier);
            group.freeListState = after;
        }
        //---------------------------------
        // FreeCounter
        //---------------------------------
        if (group.freeCounterState != after) {
            D3D12_RESOURCE_BARRIER freecounterbarrier{};
            freecounterbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            freecounterbarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            freecounterbarrier.Transition.pResource = group.freeCounterResource.Get();
            freecounterbarrier.Transition.StateBefore = group.freeCounterState;
            freecounterbarrier.Transition.StateAfter = after;
            freecounterbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barriers.push_back(freecounterbarrier);
            group.freeCounterState = after;
        }

        // Barrier 実行        
        if (!barriers.empty()) {
            dxCommon_->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
        }
    }

    void ParticleManager::TransitionParticleState(ParticleGroup& group, D3D12_RESOURCE_STATES after) {
        if (group.particleState == after) {
            return;
        }

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = group.Resource.Get();
        barrier.Transition.StateBefore = group.particleState;
        barrier.Transition.StateAfter = after;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
        group.particleState = after;
    }
    void ParticleManager::TransitionToDrawState(ParticleGroup& group) {
        std::vector<D3D12_RESOURCE_BARRIER> barriers;

        //---------------------------------
        // ParticleBuffer
        //---------------------------------
        D3D12_RESOURCE_STATES drawState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        if (group.particleState != drawState) {
            D3D12_RESOURCE_BARRIER b{};
            b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            b.Transition.pResource = group.Resource.Get();
            b.Transition.StateBefore = group.particleState;
            b.Transition.StateAfter = drawState;
            b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barriers.push_back(b);
            group.particleState = drawState;
        }

        //---------------------------------
        // FreeList
        //---------------------------------
        if (group.freeListState != D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE) {
            D3D12_RESOURCE_BARRIER b{};
            b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            b.Transition.pResource = group.freeListResource.Get();
            b.Transition.StateBefore = group.freeListState;
            b.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barriers.push_back(b);
            group.freeListState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        }

        //---------------------------------
        // FreeCounter
        //---------------------------------
        if (group.freeCounterState != D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE) {
            D3D12_RESOURCE_BARRIER b{};
            b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            b.Transition.pResource = group.freeCounterResource.Get();
            b.Transition.StateBefore = group.freeCounterState;
            b.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barriers.push_back(b);
            group.freeCounterState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        }

        if (!barriers.empty()) {
            dxCommon_->GetCommandList()->ResourceBarrier((UINT)barriers.size(), barriers.data());
        }
    }

    void ParticleManager::PipelineUAVBarriers(const std::vector<ID3D12Resource*>& resources) {
        if (resources.empty()) { return; }

        std::vector<D3D12_RESOURCE_BARRIER> barriers;
        barriers.reserve(resources.size());

        for (auto* resource : resources) {
            if (!resource) { continue; }
            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.UAV.pResource = resource;
            barriers.push_back(barrier);
        }

        if (!barriers.empty()) {
            dxCommon_->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
        }
    }
}