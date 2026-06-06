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

using namespace Microsoft::WRL;
using namespace MyEngine::MatrixVector;
using namespace MyEngine::ResourceFactory;

namespace MyEngine {
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
        // カメラリソースの生成、初期化
        CameraForGPUGenerate();
        particleInfoResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(ParticleInfo));
        particleInfoResource_->Map(0, nullptr, reinterpret_cast<void**>(&particleInfoData_));
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

    void ParticleManager::Update() {
        // カメラを CameraManager 経由で取得
        Camera* activeCamera = CameraManager::GetInstance()->GetActiveCamera();
        if (!activeCamera) { return; }

        //=========================================================
        // カメラ行列更新（GPUへ渡す）
        //=========================================================

        //----------------------------------------
        // billboard更新
        //----------------------------------------
        Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, activeCamera->GetWorldMatrix());
        // パーティクルの位置をカメラの方向に合わせるために設定
        billboardMatrix.m[3][0] = 0.0f;
        billboardMatrix.m[3][1] = 0.0f;
        billboardMatrix.m[3][2] = 0.0f;

        cameraData->view = activeCamera->GetViewMatrix();
        cameraData->projection = activeCamera->GetProjectionMatrix();
        cameraData->billboard = billboardMatrix;

        // ─── 💡 追加：マネージャ直接の自動発生処理 ───
        if (isAutoEmitActive_) {
            emitTimer_ += 1.0f / 60.0f; // 1フレーム分の時間を進める
            if (emitTimer_ >= emitFrequency_) {
                emitTimer_ = 0.0f; // タイマーリセット
                // 自分自身の Emit を直接呼び出す
                Emit(autoEmitGroup_, emitTransform_, emitColor_, emitCount_, emitVelocity_, emitParticleLifetime_);
            }
        }

        //--------------------------------------
        // DescriptorHeap設定
        //----------------------------------------
        srvmanager_->PreDraw();

        particleCommon_->CommandCompute();

        for (auto& [name, group] : particleGroups) {
            //----------------------------------------
            // UAV設定
            //----------------------------------------
            srvmanager_->SetComputeRootDescriptorTable(0, group.uavIndex);

            particleInfoData_->particleCount = MaxInstanceCount;
            dxCommon_->GetCommandList()->SetComputeRootConstantBufferView(1, particleInfoResource_->GetGPUVirtualAddress());

            //----------------------------------------
            // Dispatch
            //----------------------------------------
            uint32_t threadGroupCount = (MaxInstanceCount + 255) / 256;
            dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);
        }
    }

    void ParticleManager::Draw() {
        // パーティクルグループごとに描画処理を行う
        for (const auto& [name, particleGroup] : particleGroups) {
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
        // すでにテクスチャがロードされているか確認
        if (!TextureManager::GetInstance()->IsTextureLoaded(textureFilepath)) {
            // マテリアルのテクスチャファイルをロード
            TextureManager::GetInstance()->LoadTexture(textureFilepath);
        }

        // パーティクルグループ名が既に存在するかチェック
        auto it = particleGroups.find(name);
        if (it != particleGroups.end()) {
            // 名前が一致するグループが存在する場合、そのグループのテクスチャが一致するか確認
            if (it->second.materialData.textureFilePath == textureFilepath) {
                // テクスチャが一致する場合、既存のグループを再利用
                return;
            } else {
                // テクスチャが異なる場合、既存のグループを更新
                it->second.materialData.textureFilePath = textureFilepath;
                it->second.materialData.textureindex = TextureManager::GetInstance()->GetSrvIndex(textureFilepath);
                // 必要に応じてリソースの再割り当てなどを行うことができます
            }
        } else {
            // 新しいパーティクルグループを作成
            ParticleGroup& newGroup = particleGroups[name];
            // モデルの生成、初期化
            newGroup.model = std::make_unique<ParticleModel>();
            newGroup.model->Initialize(dxCommon_, filename);

            // 新しいパーティクルグループにテクスチャパスとインデックスを設定
            newGroup.materialData.textureFilePath = textureFilepath;
            newGroup.materialData.textureindex = TextureManager::GetInstance()->GetSrvIndex("Resources/" + textureFilepath);
            // GPU StructuredBuffer
            newGroup.Resource = CreateStructuredBufferResource(dxCommon_->GetDevice(), sizeof(ParticleForGPU) * MaxInstanceCount);
            // CPU UploadBuffer            
            newGroup.uploadResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(ParticleForGPU) * MaxInstanceCount);
            // Map (CPU側だけ)
            newGroup.uploadResource->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.particleData));

            //---------------------------------------- 
            // 初期化
            //----------------------------------------     
            for (uint32_t i = 0; i < MaxInstanceCount; ++i) {
                auto& particle = newGroup.particleData[i];
                particle.translate = { 0.0f, 0.0f, 0.0f };
                particle.pad0 = 0.0f; // 💡 追加
                particle.rotate = { 0.0f, 0.0f, 0.0f };
                particle.pad1 = 0.0f; // 💡 追加
                particle.scale = { 1.0f, 1.0f, 1.0f };
                particle.pad2 = 0.0f; // 💡 追加

                particle.color = { 1.0f, 1.0f, 1.0f, 0.0f };

                particle.velocityTranslate = { 0.0f, 0.0f, 0.0f };
                particle.pad3 = 0.0f; // 💡 追加
                particle.velocityRotate = { 0.0f, 0.0f, 0.0f };
                particle.pad4 = 0.0f; // 💡 追加
                particle.velocityScale = { 0.0f, 0.0f, 0.0f };
                particle.pad5 = 0.0f; // 💡 追加

                particle.lifetime = 0.0f;
                particle.currentTime = 0.0f;
                particle.useGravity = 0;
                particle.pad6 = 0.0f; // 💡 追加
            }

            // 初回コピー       
            dxCommon_->GetCommandList()->CopyBufferRegion(newGroup.Resource.Get(), 0, newGroup.uploadResource.Get(), 0, sizeof(ParticleForGPU) * MaxInstanceCount);
            // インスタンスバッファ用のSRVを割り当て、インデックスを記録
            newGroup.srvindex = srvmanager_->Allocate();
            // 構造体バッファ用のSRVを作成
            srvmanager_->CreateSRVforStructuredBuffer(newGroup.srvindex, newGroup.Resource.Get(), MaxInstanceCount, sizeof(ParticleForGPU));
            // UAV
            newGroup.uavIndex = srvmanager_->Allocate();
            srvmanager_->CreateUAVForStructuredBuffer(newGroup.uavIndex, newGroup.Resource.Get(), MaxInstanceCount, sizeof(ParticleForGPU));
        }
    }

    void ParticleManager::Emit(const std::string& name, const Transform& transform, const Vector4& color, uint32_t count, const Velocity& velocity, float lifetime) {

        auto it = particleGroups.find(name);
        if (it == particleGroups.end()) {
            throw std::runtime_error("Particle group not found: " + name);
        }

        ParticleGroup& group = it->second;

        // GPU側のバッファ（にマップされたCPUアドレス）に直接書き込む
        for (uint32_t i = 0; i < count; ++i) {
            // 配列の最大数を超えたら0に戻る（古いものから順に上書き）
            uint32_t index = group.lastAllocatedIndex;
            group.lastAllocatedIndex = (group.lastAllocatedIndex + 1) % MaxInstanceCount;

            auto& gpu = group.particleData[index];

            // 初期値のセット
            gpu.translate = transform.translate;
            gpu.rotate = transform.rotate;
            gpu.scale = transform.scale;
            gpu.color = color;
            gpu.velocityTranslate = velocity.translate;
            gpu.velocityRotate = velocity.rotate;
            gpu.velocityScale = velocity.scale;
            gpu.lifetime = lifetime;
            gpu.currentTime = 0.0f; // ★ 0からスタート（生存フラグを兼ねる）
            gpu.useGravity = (name == "Firework") ? 1 : 0;
            // ─── 💡 【復活】新しく出た1つ分だけを即座にGPUに送り込む ───
            UINT64 offset = static_cast<UINT64>(sizeof(ParticleForGPU)) * index;
            dxCommon_->GetCommandList()->CopyBufferRegion(group.Resource.Get(), offset, group.uploadResource.Get(), offset, sizeof(ParticleForGPU));
       }    
    }
}