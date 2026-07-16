#include "StageManager.h"
#include <ModelManager.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <TextureManager.h>
#include <SceneManager.h>
#include <SceneName.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Textures;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {

    // 静的メンバ変数の定義
    std::unique_ptr<StageManager> StageManager::instance_ = nullptr;

    // シングルトンインスタンスの取得
    StageManager* StageManager::GetInstance() {
        if (!instance_) {
            instance_ = std::make_unique<StageManager>();
        }
        return instance_.get();
    }

    // 終了
    void StageManager::Finalize() {
        instance_.reset();  // `delete` 不要
    }
    ///====================================================
    /// 初期化処理
    ///====================================================
    void StageManager::Initialize() {
		// テクスチャとモデルの読み込み
        TextureManager::GetInstance()->LoadTexture(stage::CubemapBox);
        ModelManager::GetInstance()->LoadModel(Object::Tile);
        ModelManager::GetInstance()->LoadModel(Object::goal);
        
		// スカイボックスの生成
        skybox_ = Skybox::Create(stage::CubemapBox, Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });
		// 地面の生成
        grass = Object3d::Create(Object::Tile, Transform({ 1000.0f, 1.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, -120.0f, 0.0f }));
        // クリアゲートの生成
        clearwall_ = Object3d::Create(Object::goal, Transform({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    }

    void StageManager::Update() {

        // Skyboxの回転
        Transform skyTrans = skybox_->GetTransform();
        skyTrans.rotate.y += 0.001f; // Y軸回転
        skybox_->SetRotate(skyTrans.rotate);
        skybox_->Update();

        // スカイボックス更新処理
        skybox_->Update();
        // 地面の更新処理
        grass->Update();
        // クリアゲートの更新処理
        clearwall_->Update();
    }

    void StageManager::Draw() {
        if (SceneManager::GetInstance()->IsCurrentScene(SceneName::GAMEPLAY)) {
            // クリアゲートの描画処理
            clearwall_->Draw();
        }
		// 地面の描画処理
        grass->Draw();

    }

    void StageManager::DDSDraw() {
		// スカイボックスの描画処理
        skybox_->Draw();
    }
}