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
    std::unique_ptr<StageManager> StageManager::instance = nullptr;

    // シングルトンインスタンスの取得
    StageManager* StageManager::GetInstance() {
        if (!instance) {
            instance = std::make_unique<StageManager>();
        }
        return instance.get();
    }

    // 終了
    void StageManager::Finalize() {
        instance.reset();  // `delete` 不要
        // 地面オブジェクトの解放
        //grass.reset();
        // デバッグオブジェクトのクリア
       // debugObjects_.parts();
        // その他の3Dオブジェクトもクリア
     //   object3ds_.parts();
    }

    void StageManager::Initialize() {
        // LevelLoader のインスタンスを生成s
        //loader_ = new CharacterLoader();
        //levelData_ = loader_->LoadFile("stage");  // ←ここでロード


        TextureManager::GetInstance()->LoadTexture(stage::CubemapBox);
        ModelManager::GetInstance()->LoadModel(Object::Tile);

        // オブジェクトの作成
        // 地面の作成

        skybox_ = Skybox::Create(stage::CubemapBox, Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });

        grass = Object3d::Create(Object::Tile, Transform({ 1000.0f, 1.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, -120.0f, 0.0f }));

    }

    void StageManager::Update() {
#ifdef _DEBUG
        if (showDebugObjects_) {
            // 各オブジェクトの更新
            for (auto& obj : debugObjects_) {
                obj->Update();
            }
        }
#endif

        //if (SceneManager::GetInstance()->GetCurrentScene(SceneName::TITLE)) {
        //    // Skyboxの回転
        //    Transform skyTrans = skybox_->GetTransform();
        //    skyTrans.rotate.y += 0.001f; // Y軸回転（1フレームごとに少しずつ）
        //    skybox_->SetRotate(skyTrans.rotate);
        //    skybox_->Update();
        //}
        //if (SceneManager::GetInstance()->IsCurrentScene("GAMEPLAY")) {
        //    // 地面    
        //    grass->Update();
        //}

        skybox_->Update();

        grass->Update();
    }

    void StageManager::Draw() {
#ifdef _DEBUG
        if (showDebugObjects_) {
            // 各オブジェクトの描画
            for (auto& obj : debugObjects_) {
                obj->Draw();
            }
        }
#endif
        //if (SceneManager::GetInstance()->IsCurrentScene("GAMEPLAY")) {
        //    grass->Draw();
        //}

        grass->Draw();
    }

    void StageManager::DDSDraw() {
        //if (SceneManager::GetInstance()->IsCurrentScene("TITLE")) {
        //    skybox_->Draw();
        //}
        skybox_->Draw();
    
    }
}