#include "GameOverScene.h"
#include<SceneManager.h>
#include<TextureManager.h>
#include<ModelManager.h>
#include<SpriteCommon.h>
#include<Object3dCommon.h>
#include <CameraManager.h>
#include<Input.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include <ParticleCommon.h>
#include<SkyboxCommon.h>
#include<FadeManager.h>
#include <random>
///====================================================
/// 終了処理
///====================================================
void GameOverScene::Finalize() {
    FadeManager::GetInstance()->Finalize();
}
///====================================================
///  初期化処理
///====================================================
void GameOverScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    CameraManager::GetInstance()->SetCameraMode(CameraMode::Default);
    // モデルの読み込み    
    ModelManager::GetInstance()->LoadModel("Gameover/parts_01.obj");

    // テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture("TitleEnter.png");
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
    ui1_ = Sprite::Create("TitleEnter.png", Vector2{ 500.0f, 500.0f }, 0.0f, Vector2{ 250.0f,90.0f });
    ui1_->SetTextureSize(Vector2{ 250.0f,90.0f });
    Box_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });

    // ランダムXを生成（-0.5～0.5の範囲）
    std::random_device rd;
    std::mt19937 randomEngine(rd());
    std::uniform_real_distribution<float> transformY(5.0f, 10.0f);
    std::uniform_real_distribution<float> distX(-12.0f, 10.0f);
    std::uniform_real_distribution<float> distY(0.01f, 0.03f);
    std::uniform_real_distribution<float> rotateX(-0.01f, 0.01f);
    std::uniform_real_distribution<float> rotateY(-0.01f, 0.01f);
    std::uniform_real_distribution<float> rotateZ(-0.01f, 0.01f);

    std::vector<std::string> textureNames = {
    "Gameover/parts_01.obj",
    "Gameover/parts_01.obj",
    "Gameover/parts_01.obj",     
    "Gameover/parts_01.obj", 
    "Gameover/parts_01.obj",
    };
    for (int i = 0; i < partCount_; ++i) {
        PartInfo part;

        part.transform = { {1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {distX(randomEngine), transformY(randomEngine), 30.0f} };
        // 1つ目はそのまま、それ以降は2個ずつテクスチャを切り替え
        int textureIndex = 0; // 1つ目は 0
        if (i > 0) {
            textureIndex = ((i - 1) / 2) + 1; // 2個ごとに切り替え
            if (textureIndex >= textureNames.size()) textureIndex = textureNames.size() - 1; // 配列オーバー防止
        }
           
        part.obj = Object3d::Create("Gameover/parts_01.obj", part.transform);
        part.obj->SetModel(textureNames[textureIndex]); // 後からテクスチャを設定

        part.fallSpeed = { 0.0f, distY(randomEngine), 0.0f }; // Y軸落下
        part.rotateSpeed = { rotateX(randomEngine), rotateY(randomEngine), rotateZ(randomEngine) };

        partsList.push_back(std::move(part));
    }

    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();
}
///====================================================
/// 更新処理
///====================================================
void GameOverScene::Update() {  
    // フェードイン開始
    if (!FadeManager::GetInstance()->IsFadeStart() && !FadeManager::GetInstance()->IsFading()) {
        // フェード開始
        FadeManager::GetInstance()->StartFadeIn(1.0f, FadeStyle::Normal);
    }
    // フェードマネージャの更新処理
    FadeManager::GetInstance()->Update();
 
    // ゲームシーンへの入力処理
    if (Input::GetInstance()->Triggrkey(DIK_RETURN) && !FadeManager::GetInstance()->IsFading() && FadeManager::GetInstance()->IsFadeEnd()) {
        // フェード開始
        FadeManager::GetInstance()->StartFadeOut(1.0f,FadeStyle::SilhouetteSlide);
    }

    // フェードアウト完了後にタイトルへ
    if (FadeManager::GetInstance()->IsFadeEnd() && FadeManager::GetInstance()->GetFadeType() == FadeType::FadeOut) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理
    Box_->Update();   // 背景の更新

    for (PartInfo& part : partsList) {
        // 落下
        part.transform.translate.y -= part.fallSpeed.y;
        if (part.transform.translate.y < -10.0f) {
            part.transform.translate.y = -10.0f; // 地面で止める
        }

        // 回転
        part.transform.rotate -= part.rotateSpeed;

        // 更新反映
        part.obj->SetTranslate(part.transform.translate);
        part.obj->SetRotate(part.transform.rotate);
        part.obj->Update();
    }

#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理

    ui1_->Update(); // スプライトの更新

#pragma endregion 全てのSprite個々の更新処理
#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI

    // Camera
    CameraManager::GetInstance()->DrawImGui();


#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了

}
///====================================================
/// 描画処理
///====================================================
void GameOverScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    Box_->Draw(); // 背景の描画

    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();
    
    for (PartInfo& part : partsList) {
        part.obj->Draw();
    }

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();

    ui1_->Draw(); // スプライトの描画
    
    // フェードマネージャの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}