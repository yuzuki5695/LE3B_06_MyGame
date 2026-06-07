#include "ParticleGroupfactory.h"
#include <DirectXCommon.h>
#include <SrvManager.h>
#include <TextureManager.h>
#include <ResourceFactory.h>

namespace MyEngine {

    using namespace ResourceFactory;

    // 静的メンバ変数の定義
    std::unique_ptr<ParticleGroupfactory> ParticleGroupfactory::instance = nullptr;

    // シングルトンインスタンスの取得
    ParticleGroupfactory* ParticleGroupfactory::GetInstance() {
        if (!instance) {
            instance = std::make_unique<ParticleGroupfactory>();
        }
        return instance.get();
    }

    // 終了
    void ParticleGroupfactory::Finalize() {
        instance.reset();  // `delete` 不要
    }

    bool ParticleGroupfactory::Initialize(const std::string& name, const std::string& textureFilepath, std::unordered_map<std::string, ParticleGroup>* particleGroups) {
        assert(particleGroups); // NULLチェック

        // すでにテクスチャがロードされているか確認
        if (!TextureManager::GetInstance()->IsTextureLoaded(textureFilepath)) {
            TextureManager::GetInstance()->LoadTexture(textureFilepath);
        }

        // 2. 重複チェック
        auto it = particleGroups->find(name);
        if (it != particleGroups->end()) {
            if (it->second.materialData.textureFilePath == textureFilepath) {
                return true; // 既存のものをそのまま再利用するので、新規作成は不要
            } else {
                // テクスチャが異なる場合の更新処理（本物のマップデータを書き換える）
                it->second.materialData.textureFilePath = textureFilepath;
                it->second.materialData.textureindex = TextureManager::GetInstance()->GetSrvIndex(textureFilepath);
                return true; // 更新が完了したので、新規作成は不要
            }
        }

        return false; // 既存に存在しないため、新規作成が必要
    }

    ParticleGroup ParticleGroupfactory::CreateNewGroup(DirectXCommon* dxCommon, SrvManager* srvManager, const std::string& filename, const std::string& textureFilepath, uint32_t maxInstanceCount) {
        ParticleGroup newGroup;

        // 1. モデルの生成、初期化
        newGroup.model = std::make_unique<ParticleModel>();
        newGroup.model->Initialize(dxCommon, filename);

        // 2. テクスチャパスとインデックスを設定
        newGroup.materialData.textureFilePath = textureFilepath;
        newGroup.materialData.textureindex = TextureManager::GetInstance()->GetSrvIndex("Resources/" + textureFilepath);

        // 3. GPU StructuredBuffer 生成
        newGroup.Resource = ResourceFactory::CreateStructuredBufferResource(dxCommon->GetDevice(), sizeof(ParticleForGPU) * maxInstanceCount);
        newGroup.currentState = D3D12_RESOURCE_STATE_COMMON;

        // 4. CPU UploadBuffer 生成 & ゼロクリア
        newGroup.uploadResource = ResourceFactory::CreateBufferResource(dxCommon->GetDevice(), sizeof(ParticleForGPU) * maxInstanceCount);

        if (SUCCEEDED(newGroup.uploadResource->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.particleData)))) {
            std::memset(newGroup.particleData, 0, sizeof(ParticleForGPU) * maxInstanceCount);
            newGroup.uploadResource->Unmap(0, nullptr);
        }

        // 5. GPU側リソースへ初期データをコピー
        dxCommon->GetCommandList()->CopyBufferRegion(
            newGroup.Resource.Get(), 0, newGroup.uploadResource.Get(), 0, sizeof(ParticleForGPU) * maxInstanceCount
        );
        newGroup.currentState = D3D12_RESOURCE_STATE_COPY_DEST;

        // 6. SRV の割り当てと作成
        newGroup.srvindex = srvManager->Allocate();
        srvManager->CreateSRVforStructuredBuffer(newGroup.srvindex, newGroup.Resource.Get(), maxInstanceCount, sizeof(ParticleForGPU));

        // 7. UAV の割り当てと作成
        newGroup.uavIndex = srvManager->Allocate();
        srvManager->CreateUAVForStructuredBuffer(newGroup.uavIndex, newGroup.Resource.Get(), maxInstanceCount, sizeof(ParticleForGPU));

        // 初期化済みのグループを返す
        return newGroup;
    }
}