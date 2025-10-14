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

void GameOverScene::Finalize() {
    FadeManager::GetInstance()->Finalize();
}

void GameOverScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    CameraManager::GetInstance()->SetCameraMode(CameraMode::Default);
    ModelManager::GetInstance()->LoadModel("GameOver.obj");

    TextureManager::GetInstance()->LoadTexture("TitleEnter.png");

    ui1_ = Sprite::Create("TitleEnter.png", Vector2{ 500.0f, 500.0f }, 0.0f, Vector2{ 250.0f,90.0f });
    ui1_->SetTextureSize(Vector2{ 250.0f,90.0f });


    clear = Object3d::Create("GameOver.obj", Transform{ { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 30.0f } });
    Box_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });


    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();
}

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
    Box_->Update();

    clear->Update();

#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理

    
    ui1_->Update();

#pragma endregion 全てのSprite個々の更新処理
#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI

    // Camera
    CameraManager::GetInstance()->DrawImGui();


#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了

}

void GameOverScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    Box_->Draw();

    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();
    
    clear->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();

    ui1_->Draw();
    
    // フェードマネージャの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}