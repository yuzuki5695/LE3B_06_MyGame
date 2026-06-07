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
        // カメラを CameraManager 経由で取得
        Camera* activeCamera = CameraManager::GetInstance()->GetActiveCamera();
        if (!activeCamera) { return; }

        //=========================================================
        // カメラ行列更新（GPUへ渡す）
        //=========================================================

        // billboard更新
        Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, activeCamera->GetWorldMatrix());
        // パーティクルの位置をカメラの方向に合わせるために設定
        billboardMatrix.m[3][0] = 0.0f;
        billboardMatrix.m[3][1] = 0.0f;
        billboardMatrix.m[3][2] = 0.0f;

        cameraData->view = activeCamera->GetViewMatrix();
        cameraData->projection = activeCamera->GetProjectionMatrix();
        cameraData->billboard = billboardMatrix;

        // DescriptorHeap設定
        srvmanager_->PreDraw();

        // 💡【追加】Compute Shaderを実行する前に、リソースをUAV状態に遷移させる
        for (auto& [name, group] : particleGroups) {
            TransitionParticleBuffer(group, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        }

        // SpawnQueue処理       
        ProcessSpawnRequests();

        // 生成CS と 更新CS の間のハザードを防ぐUAVバリア
        if (!particleGroups.empty()) {
            std::vector<D3D12_RESOURCE_BARRIER> spawnBarriers;
            spawnBarriers.reserve(particleGroups.size());

            for (auto& [name, group] : particleGroups) {
                // スポーン等に関わらず、これから更新する可能性のある全リソースにバリアを張る
                D3D12_RESOURCE_BARRIER barrier{};
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.UAV.pResource = group.Resource.Get();
                spawnBarriers.push_back(barrier);
            }
            dxCommon_->GetCommandList()->ResourceBarrier(static_cast<UINT>(spawnBarriers.size()), spawnBarriers.data());
        }

        particleCommon_->CommandCompute();

        for (auto& [name, group] : particleGroups) {
            if (group.lastAllocatedIndex == 0) { continue; }

            // UAV設定
            srvmanager_->SetComputeRootDescriptorTable(0, group.uavIndex);
            particleInfoData_->particleCount = MaxInstanceCount;
            dxCommon_->GetCommandList()->SetComputeRootConstantBufferView(1, particleInfoResource_->GetGPUVirtualAddress());
            // Dispatch
            uint32_t threadGroupCount = (MaxInstanceCount + 255) / 256;
            dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);
            UAVBarrier(group.Resource.Get());
        }
    }

    void ParticleManager::Draw() {
        // パーティクルグループごとに描画処理を行う
        for (auto& [name, particleGroup] : particleGroups) {
            // UAV -> SRV
            TransitionParticleBuffer(particleGroup, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            // モデルに必要なバッファをバインド（頂点バッファや定数バッファなど）
            particleGroup.model->Draw();
            // インスタンシングデータの SRV を設定（テクスチャファイルのパスを指定）
            srvmanager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvindex);
            // SRVで画像を表示
            srvmanager_->SetGraphicsRootDescriptorTable(2, particleGroup.materialData.textureindex);

            dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource->GetGPUVirtualAddress());
            // 描画（インスタンシング）を実行
            dxCommon_->GetCommandList()->DrawInstanced(static_cast<UINT>(particleGroup.model->GetVertexCount()), static_cast<UINT>(MaxInstanceCount), 0, 0);
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

    void ParticleManager::ProcessSpawnRequests() {
        if (spawnRequests_.empty()) { return; }

        // 1フレーム中の総リクエスト数をカウントするカウンター
        uint32_t totalRequestCount = 0;

        // グループごとの「今回の生成範囲」を一時記録するベクター
        struct ActiveGroupSpawn { ParticleGroup* group; uint32_t startIndex; uint32_t count; };
        std::vector<ActiveGroupSpawn> activeSpawns;

        // -------------------------------------------------------------
        // 1. 全てのリクエストを「CPU上で」一つの連続した配列に超高速で詰め込む
        // -------------------------------------------------------------
        // 1. CPU上での詰め込み処理（上限を MaxSpawnRequestCount に）
        for (const auto& request : spawnRequests_) {
            auto it = particleGroups.find(request.groupName);
            if (it == particleGroups.end()) { continue; }

            ParticleGroup& group = it->second;
            const auto& spawnData = request.spawnData;
            uint32_t groupStartIndex = totalRequestCount;

            for (uint32_t i = 0; i < spawnData.count; ++i) {
                if (totalRequestCount >= MaxSpawnRequestCount) { break; } // 広げた上限

                // 配列の現在の位置に直接代入（CPU上での単なるメモリ書き込みなので超高速）
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

                // ★ リングバッファのインデックス計算をここでやり、GPUに指示する
                dst.targetIndex = group.lastAllocatedIndex;
                group.lastAllocatedIndex = (group.lastAllocatedIndex + 1) % MaxInstanceCount;

                totalRequestCount++;
            }

            // このグループで実際に処理する個数
            uint32_t groupSpawnCount = totalRequestCount - groupStartIndex;
            if (groupSpawnCount > 0) {
                activeSpawns.push_back({ &group, groupStartIndex, groupSpawnCount });
            }
        }

        // リクエストキューをクリア（CPU側の仕事はこれだけで終了！）
        spawnRequests_.clear();
        if (activeSpawns.empty()) { return; }

        // 2. GPUへ命令発行
        particleCommon_->CommandSpawn();

        uint32_t cbCBVIndex = 0; // ループごとに定数バッファの書き込み先インデックスをずらす

        for (const auto& spawn : activeSpawns) {
            if (cbCBVIndex >= MaxGroupCount) { break; }

            // ★ 重要：同じメモリを上書きせず、配列の別々の要素に書き込む！
            groupSpawnCBData_[cbCBVIndex].startRequestIndex = spawn.startIndex;
            groupSpawnCBData_[cbCBVIndex].spawnCount = spawn.count;

            srvmanager_->SetComputeRootDescriptorTable(0, spawn.group->uavIndex);

            // ★ 重要：定数バッファの参照アドレスを、グループの個数分（バイトサイズ分）ずらしてGPUに渡す！
            D3D12_GPU_VIRTUAL_ADDRESS cbvAddress = groupSpawnCBResource_->GetGPUVirtualAddress() + (cbCBVIndex * sizeof(GroupSpawnCB));
            dxCommon_->GetCommandList()->SetComputeRootConstantBufferView(1, cbvAddress);

            dxCommon_->GetCommandList()->SetComputeRootShaderResourceView(2, spawnListResource_->GetGPUVirtualAddress());

            uint32_t threadGroupCount = (spawn.count + 63) / 64;
            dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);

            cbCBVIndex++;
        }

        // ★ 重要：UAVバリアは各ループで足止めせず、全グループの命令を投げ終えた後に「1回だけ」まとめて張る！
        // これにより、GPUは全グループのスポーン処理を完全に並列（同時）に実行できます。
        D3D12_RESOURCE_BARRIER barriers[MaxGroupCount];
        uint32_t barrierCount = 0;
        for (const auto& spawn : activeSpawns) {
            barriers[barrierCount].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            barriers[barrierCount].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[barrierCount].UAV.pResource = spawn.group->Resource.Get();
            barrierCount++;
        }
        dxCommon_->GetCommandList()->ResourceBarrier(barrierCount, barriers);
    }

    void ParticleManager::TransitionParticleBuffer(ParticleGroup& group, D3D12_RESOURCE_STATES after) {
        if (group.currentState == after) {
            return;
        }
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = group.Resource.Get();
        barrier.Transition.StateBefore =   group.currentState;
        barrier.Transition.StateAfter = after;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
        group.currentState = after;
    }

    void ParticleManager::UAVBarrier(ID3D12Resource* resource) {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = resource;
        dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
    }
}