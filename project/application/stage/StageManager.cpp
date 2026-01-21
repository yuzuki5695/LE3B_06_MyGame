#include "StageManager.h"
#include <ModelManager.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

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
    loader_ = new CharacterLoader();
    levelData_ = loader_->LoadFile("stage");  // ←ここでロード

    // 敵モデルをあらかじめ読み込む
   // ModelManager::GetInstance()->LoadModel("Enemy.obj");
    ModelManager::GetInstance()->LoadModel("Gameplay/Model/Tile/Tile.obj");
    
    // オブジェクトの作成
    // 地面の作成
    grass = Object3d::Create("Gameplay/Model/Tile/Tile.obj", Transform({ 1000.0f, 1.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, -8.0f, 50.0f }));

    //// レベルデータからオブジェクトを読み込む
    //for (auto& objData : levelData_->objects) {
    //    // Point_ で始まるファイル名のみ対象
    //    if (objData.fileName.rfind("Point_", 0) == 0) {

    //        Transform tr;
    //        tr.scale = objData.scaling;
    //        tr.rotate = objData.rotation;
    //        tr.translate = objData.translation;

    //        // モデルは固定で Enemy.obj を使用
    //        std::string modelName = "Enemy.obj";

    //        // Object3d を生成
    //        auto obj = Object3d::Create(modelName, tr);
    //        debugObjects_.push_back(std::move(obj));
    //    }
    //} 

    showDebugObjects_ = false; // デフォルトは非表示
#ifdef _DEBUG
    // === デバッグ可視化用オブジェクト ===
    //ModelManager::GetInstance()->LoadModel("Enemy.obj"); // 目印用モデル（SphereやCubeなど）

    //for (auto& objData : levelData_->objects) {
    //    // Point_ で始まるオブジェクトをデバッグ可視化対象にする
    //    if (objData.fileName.rfind("Point_", 0) == 0) {
    //        Transform tr;
    //        tr.scale = { 0.5f, 0.5f, 0.5f }; // 小さく目印表示
    //        tr.rotate = objData.rotation;
    //        tr.translate = objData.translation;

    //        auto debugObj = Object3d::Create("Enemy.obj", tr);
    //        debugObjects_.push_back(std::move(debugObj));
    //    }
    //}
#endif
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

    // 地面    
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


    grass->Draw();
}
    
void StageManager::DebugImGui() {
#ifdef USE_IMGUI
    ImGui::Begin("Stage Debug");
    ImGui::Checkbox("Show Debug Objects", &showDebugObjects_);
    ImGui::Text("Loaded objects: %d", (int)object3ds_.size());
    ImGui::Text("Debug points: %d", (int)debugObjects_.size());
    ImGui::End();
#endif
}