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

         //----------------------------------------
         // DescriptorHeap設定
         //----------------------------------------
        srvmanager_->PreDraw();

        particleCommon_->CommandCompute();

        for (auto& [name, group] : particleGroups) {
            if (group.kNumInstance == 0) {
                continue;
            }
            //----------------------------------------
            // UAV設定
            //----------------------------------------
            srvmanager_->SetComputeRootDescriptorTable(0, group.uavIndex);
            //----------------------------------------
            // Dispatch
            //----------------------------------------
            uint32_t threadGroupCount = (group.kNumInstance + 255) / 256;
            dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);
            // UAV barrier
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(group.Resource.Get());
            dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
            // UAV → SRV
            CD3DX12_RESOURCE_BARRIER transition =
                CD3DX12_RESOURCE_BARRIER::Transition(group.Resource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            dxCommon_->GetCommandList()->ResourceBarrier(1, &transition);
        }
    }

    void ParticleManager::Draw() {
        // パーティクルグループごとに描画処理を行う
        for (const auto& [name, particleGroup] : particleGroups) {
            // インスタンス数が0の場合は描画しない
            if (particleGroup.kNumInstance == 0) {
                continue;
            }
            // モデルに必要なバッファをバインド（頂点バッファや定数バッファなど）
            particleGroup.model->Draw();
            // インスタンシングデータの SRV を設定（テクスチャファイルのパスを指定）
            srvmanager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvindex);
            // SRVで画像を表示
            srvmanager_->SetGraphicsRootDescriptorTable(2, particleGroup.materialData.textureindex);

            dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource->GetGPUVirtualAddress());
            // 描画（インスタンシング）を実行
            dxCommon_->GetCommandList()->DrawInstanced(static_cast<UINT>(particleGroup.model->GetVertexCount()), static_cast<UINT>(particleGroup.kNumInstance), 0, 0);

            // SRV → UAV
            CD3DX12_RESOURCE_BARRIER barrier =
                CD3DX12_RESOURCE_BARRIER::Transition(particleGroup.Resource.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
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
            newGroup.kNumInstance = 0;
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

                particle.translate = { 0,0,0 };
                particle.rotate = { 0,0,0 };
                particle.scale = { 1,1,1 };

                particle.color = { 1,1,1,0 };

                particle.velocityTranslate = { 0,0,0 };
                particle.velocityRotate = { 0,0,0 };
                particle.velocityScale = { 0,0,0 };

                particle.lifetime = 0.0f;
                particle.currentTime = 0.0f;
                particle.useGravity = 0;
            }

            // 初回コピー       
            dxCommon_->GetCommandList()->CopyBufferRegion(newGroup.Resource.Get(), 0, newGroup.uploadResource.Get(), 0, sizeof(ParticleForGPU) * MaxInstanceCount);
            // SRV
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

        size_t currentParticleCount = group.particles.size();
        if (currentParticleCount + count > MaxInstanceCount) {
            count = static_cast<uint32_t>(MaxInstanceCount - currentParticleCount);
        }

        if (count == 0) return;

        for (uint32_t i = 0; i < count; ++i) {
            Particle newParticle;
            newParticle.transform.translate = { transform.translate.x, transform.translate.y, transform.translate.z };
            newParticle.transform.rotate = { transform.rotate.x, transform.rotate.y, transform.rotate.z };
            newParticle.transform.scale = { transform.scale.x, transform.scale.y, transform.scale.z };
            newParticle.color = { color.x, color.y, color.z, color.w };
            newParticle.lifetime = lifetime;
            newParticle.currentTime = 0.0f;
            newParticle.Velocity.translate = velocity.translate;
            newParticle.Velocity.rotate = velocity.rotate;
            newParticle.Velocity.scale = velocity.scale;
            newParticle.useGravity = (name == "Firework");

            // 作成したパーティクルをパーティクルリストに追加
            group.particles.push_back(newParticle);

            //--------------------------------
            // GPUへ初期値コピー            
            //--------------------------------
            uint32_t index = static_cast<uint32_t>(group.particles.size() - 1);
            auto& gpu = group.particleData[index];

            gpu.translate = transform.translate;
            gpu.rotate = transform.rotate;
            gpu.scale = transform.scale;
            gpu.color = color;

            gpu.velocityTranslate = velocity.translate;
            gpu.velocityRotate = velocity.rotate;
            gpu.velocityScale = velocity.scale;
            gpu.lifetime = lifetime;
            gpu.currentTime = 0.0f;
            gpu.useGravity = newParticle.useGravity ? 1 : 0;
        }
        // 描画で使用するインスタンス数を更新
        group.kNumInstance = static_cast<uint32_t>(group.particles.size());
        {
            CD3DX12_RESOURCE_BARRIER barrier =
                CD3DX12_RESOURCE_BARRIER::Transition(group.Resource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);
            dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
        }
        // GPUへ一括コピー
        dxCommon_->GetCommandList()->CopyBufferRegion(group.Resource.Get(), 0, group.uploadResource.Get(), 0, sizeof(ParticleForGPU) * MaxInstanceCount);
        {
            CD3DX12_RESOURCE_BARRIER barrier =
                CD3DX12_RESOURCE_BARRIER::Transition(group.Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
        }
    }
}