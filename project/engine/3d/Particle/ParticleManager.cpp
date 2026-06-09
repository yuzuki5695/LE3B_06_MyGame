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
        CameraForGPUGenerate();       // カメラ用
        ParticleInfoBufferGenerate(); // パーティクル情報用
        SpawnListBufferGenerate();    // スポーン要求リスト用
        GroupSpawnCBufferGenerate();  // グループスポーン定数バッファ用
    }

    void ParticleManager::CameraForGPUGenerate() {
        // カメラ用リソースを作る
        cameraResource = CreateBufferResource(particleCommon_->GetDxCommon()->GetDevice(), sizeof(CameraData));
        // 書き込むためのアドレスを取得
        cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
        // カメラを CameraManager 経由で取得
        cameraData->view = MakeIdentity4x4();
        cameraData->projection = MakeIdentity4x4();
        cameraData->billboard = MakeIdentity4x4();
    }

    void ParticleManager::ParticleInfoBufferGenerate() {
        // 最大インスタンス数などを Compute Shader に伝えるためのバッファ
        particleInfoResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(ParticleInfo));
        particleInfoResource_->Map(0, nullptr, reinterpret_cast<void**>(&particleInfoData_));
    }

    void ParticleManager::SpawnListBufferGenerate() {
        // 1フレーム中に発生した全グループのスポーンリクエストを一時的に集約するバッファ
        spawnListResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(SpawnRequestGPU) * MaxSpawnRequestCount);
        spawnListResource_->Map(0, nullptr, reinterpret_cast<void**>(&spawnListData_));
    }

    void ParticleManager::GroupSpawnCBufferGenerate() {
        // 各グループが上の「SpawnList」のどこから何個読み込めばいいかのインデックス情報を格納
        groupSpawnCBResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(GroupSpawnCB) * MaxGroupCount);
        groupSpawnCBResource_->Map(0, nullptr, reinterpret_cast<void**>(&groupSpawnCBData_));
    }

    void ParticleManager::Update() {
        Matrix4x4 billboardMatrix;

        // カメラを CameraManager 経由で取得
        Camera* activeCamera = CameraManager::GetInstance()->GetActiveCamera();
        if (!activeCamera) { return; }

        // カメラ行列更新（GPUへ渡す）
        billboardMatrix = Multiply(backToFrontMatrix, activeCamera->GetWorldMatrix());
        cameraData->view = activeCamera->GetViewMatrix();
        cameraData->projection = activeCamera->GetProjectionMatrix();
        // パーティクルの位置をカメラの方向に合わせるために設定
        billboardMatrix.m[3][0] = 0.0f;
        billboardMatrix.m[3][1] = 0.0f;
        billboardMatrix.m[3][2] = 0.0f;
        cameraData->billboard = billboardMatrix;

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
            for (const auto& spawn : activeSpawns) { targetResources.push_back(spawn.group->Resource.Get()); }
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
            //            if (group.lastAllocatedIndex == 0) { continue; }
            if (group.lastAllocatedIndex == 0 && group.activeInstanceCount == 0) { continue; }

            srvmanager_->SetComputeRootDescriptorTable(0, group.uavIndex);
            // 💡 常に MaxInstanceCount ではなく、これまでに割り当てられた最大インデックス（またはMax）までに制限       
            // 一度でもMaxまで回ったらMaxInstanceCountになりますが、未使用時はlastAllocatedIndex付近に絞れます
            uint32_t processCount = (group.lastAllocatedIndex > group.activeInstanceCount) ? group.lastAllocatedIndex : group.activeInstanceCount;
            if (processCount == 0) processCount = MaxInstanceCount; // 安全策
            particleInfoData_->particleCount = processCount;

            dxCommon_->GetCommandList()->SetComputeRootConstantBufferView(1, particleInfoResource_->GetGPUVirtualAddress());

            uint32_t threadGroupCount = (processCount + 255) / 256;
            dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);
            updateResources.push_back(group.Resource.Get());
        }

        if (!updateResources.empty()) {
            PipelineUAVBarriers(updateResources);
        }
    }

    void ParticleManager::Draw() {
        // パーティクルグループごとに描画処理を行う
        for (auto& [name, particleGroup] : particleGroups) {
            // 1つも生成されていない（インデックスが0）なら、描画処理そのものをスキップしてGPUを休ませる
            if (particleGroup.lastAllocatedIndex == 0) { continue; }
            // UAV -> SRV
            TransitionParticleBuffer(particleGroup, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            // インスタンシングデータの SRV を設定（テクスチャファイルのパスを指定）
            srvmanager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvindex);
            // SRVで画像を表示
            srvmanager_->SetGraphicsRootDescriptorTable(2, particleGroup.materialData.textureindex);
            // カメラ用定数バッファをセット
            dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource->GetGPUVirtualAddress());
            // モデルに必要なバッファをバインド（頂点バッファや定数バッファなど）
            particleGroup.model->Draw();
            // 描画（インスタンシング）を実行
            dxCommon_->GetCommandList()->DrawInstanced(static_cast<UINT>(particleGroup.model->GetVertexCount()), static_cast<UINT>(particleGroup.lastAllocatedIndex), 0, 0);
        }
    }

    void ParticleManager::CreateParticleGroup(const std::string& name, const std::string& textureFilepath, const std::string& filename) {
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

                SpawnRequestGPU& dst = spawnListData_[totalRequestCount];
                dst.translate = spawnData.transform.translate;
                dst.rotate = spawnData.transform.rotate;
                dst.scale = spawnData.transform.scale;
                dst.color = spawnData.color;
                dst.velocityTranslate = spawnData.velocity.translate;
                dst.velocityRotate = spawnData.velocity.rotate;
                dst.velocityScale = spawnData.velocity.scale;
                dst.lifetime = spawnData.lifetime;
                dst.useGravity = spawnData.useGravity ? 1u : 0u;

                dst.targetIndex = group.lastAllocatedIndex;
                group.lastAllocatedIndex = (group.lastAllocatedIndex + 1) % MaxInstanceCount;
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

            groupSpawnCBData_[cbCBVIndex].startRequestIndex = spawn.startIndex;
            groupSpawnCBData_[cbCBVIndex].spawnCount = spawn.count;

            srvmanager_->SetComputeRootDescriptorTable(0, spawn.group->uavIndex);
            D3D12_GPU_VIRTUAL_ADDRESS cbvAddress = groupSpawnCBResource_->GetGPUVirtualAddress() + (cbCBVIndex * sizeof(GroupSpawnCB));
            dxCommon_->GetCommandList()->SetComputeRootConstantBufferView(1, cbvAddress);
            dxCommon_->GetCommandList()->SetComputeRootShaderResourceView(2, spawnListResource_->GetGPUVirtualAddress());

            uint32_t threadGroupCount = (spawn.count + 63) / 64;
            dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);

            cbCBVIndex++;
        }
    }

    void ParticleManager::TransitionParticleBuffer(ParticleGroup& group, D3D12_RESOURCE_STATES after) {
        if (group.currentState == after) {
            return;
        }
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = group.Resource.Get();
        barrier.Transition.StateBefore = group.currentState;
        barrier.Transition.StateAfter = after;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
        group.currentState = after;
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