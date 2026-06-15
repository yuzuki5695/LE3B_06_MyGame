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

        // 最大インスタンスの設定
        newGroup.maxInstanceCount = maxInstanceCount;

        // 3. GPU StructuredBuffer 生成
        newGroup.Resource = CreateStructuredBufferResource(dxCommon->GetDevice(), sizeof(ParticleForGPU) * maxInstanceCount);
        newGroup.particleState = D3D12_RESOURCE_STATE_COMMON;

        // 4. CPU UploadBuffer 生成 & ゼロクリア
        newGroup.uploadResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(ParticleForGPU) * maxInstanceCount);
        void* mapped = nullptr;
        newGroup.uploadResource->Map(0, nullptr, &mapped);
        memset(mapped, 0, sizeof(ParticleForGPU) * maxInstanceCount);
        newGroup.uploadResource->Unmap(0, nullptr);
        // 5. GPU側リソースへ初期データをコピー
        dxCommon->GetCommandList()->CopyBufferRegion(newGroup.Resource.Get(), 0, newGroup.uploadResource.Get(), 0, sizeof(ParticleForGPU) * maxInstanceCount);
        newGroup.particleState = D3D12_RESOURCE_STATE_COPY_DEST;
        //=====================================
        // FreeList 初期化
        //=====================================
        newGroup.freeListResource = CreateStructuredBufferResource(dxCommon->GetDevice(), sizeof(uint32_t) * maxInstanceCount);
        newGroup.freeListState = D3D12_RESOURCE_STATE_COMMON;
        newGroup.freeListUavIndex = srvManager->Allocate();
        srvManager->CreateUAVForStructuredBuffer(newGroup.freeListUavIndex, newGroup.freeListResource.Get(), maxInstanceCount, sizeof(uint32_t));
        std::vector<uint32_t> freeListInit(maxInstanceCount);
        for (uint32_t i = 0; i < maxInstanceCount; ++i) {
            freeListInit[i] = i;
        }

        // Upload
        newGroup.freeListUpload = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t) * maxInstanceCount);
        mapped = nullptr;
        newGroup.freeListUpload->Map(0, nullptr, &mapped);
        memcpy(mapped, freeListInit.data(), sizeof(uint32_t) * maxInstanceCount);
        newGroup.freeListUpload->Unmap(0, nullptr);
        dxCommon->GetCommandList()->CopyBufferRegion(newGroup.freeListResource.Get(), 0, newGroup.freeListUpload.Get(), 0, sizeof(uint32_t) * maxInstanceCount);
        newGroup.freeListState = D3D12_RESOURCE_STATE_COPY_DEST;
        //=====================================
        // Counter 初期化
        //=====================================
        newGroup.freeCounterResource = CreateStructuredBufferResource(dxCommon->GetDevice(), sizeof(uint32_t));
        newGroup.freeCounterState = D3D12_RESOURCE_STATE_COMMON;
        newGroup.freeCounterUavIndex = srvManager->Allocate();
        srvManager->CreateUAVForStructuredBuffer(newGroup.freeCounterUavIndex, newGroup.freeCounterResource.Get(), 1, sizeof(uint32_t));
        uint32_t initialCount = maxInstanceCount;
        // Upload
        newGroup.counterUpload = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t));
        mapped = nullptr;
        newGroup.counterUpload->Map(0, nullptr, &mapped);
        memcpy(mapped, &initialCount, sizeof(uint32_t));
        newGroup.counterUpload->Unmap(0, nullptr);
        dxCommon->GetCommandList()->CopyBufferRegion(newGroup.freeCounterResource.Get(), 0, newGroup.counterUpload.Get(), 0, sizeof(uint32_t));
        newGroup.freeCounterState = D3D12_RESOURCE_STATE_COPY_DEST;

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