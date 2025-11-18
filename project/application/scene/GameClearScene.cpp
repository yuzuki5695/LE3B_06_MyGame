#include "GameClearScene.h"
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
#include <FadeManager.h>

///====================================================
/// 終了処理
///====================================================
void GameClearScene::Finalize() {
    FadeManager::GetInstance()->Finalize();    //  フェードマネージャの解放処理
}
///====================================================
///  初期化処理
///====================================================
void GameClearScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    CameraManager::GetInstance()->SetCameraMode(CameraMode::ClearCamera);
    // モデルの読み込み
    ModelManager::GetInstance()->LoadModel("Clear.obj");
    ModelManager::GetInstance()->LoadModel("Gameplay/Model/Player/Player.obj");

    // テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
    TextureManager::GetInstance()->LoadTexture("Gameclear/Texture/UI_01.png");

    ui1_ = Sprite::Create("Gameclear/Texture/UI_01.png", Vector2{ 650.0f, 500.0f }, 0.0f, Vector2{ 360.0f,100.0f });
    ui1_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準
    ui1_->SetTextureSize(Vector2{ 360.0f,100.0f });

    clear = Object3d::Create("Clear.obj", Transform{ { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 30.0f } });
    Box_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });



    player_ =  Object3d::Create("Gameplay/Model/Player/Player.obj", Transform{ { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 30.0f } });

    offset_ = { 0.0f,0.0f,30.0f };

    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();
}

///====================================================
/// 更新処理
///====================================================
void GameClearScene::Update() { 
    // フェードイン開始
    if (!FadeManager::GetInstance()->IsFadeStart() && !FadeManager::GetInstance()->IsFading()) {
        // フェード開始
        FadeManager::GetInstance()->StartFadeIn(1.0f, FadeStyle::Normal);
    }
    // フェードマネージャの更新処理
    FadeManager::GetInstance()->Update();
 
    //// ゲームシーンへの入力処理
    //if (Input::GetInstance()->Triggrkey(DIK_RETURN) && !FadeManager::GetInstance()->IsFading() && FadeManager::GetInstance()->IsFadeEnd()) {
    //    // フェード開始
    //    FadeManager::GetInstance()->StartFadeOut(1.0f,FadeStyle::SilhouetteSlide);
    //}

    //// フェードアウト完了後にタイトルへ
    //if (FadeManager::GetInstance()->IsFadeEnd() && FadeManager::GetInstance()->GetFadeType() == FadeType::FadeOut) {
    //    SceneManager::GetInstance()->ChangeScene("TITLE");
    //}

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/ 
    CameraManager::GetInstance()->SetTarget(player_.get());
    CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理    

	Box_->Update();   // 背景の更新
	clear->Update(); // オブジェクトの更新

    offset_.y += 0.01f;
    player_->SetTranslate(offset_);
    player_->Update();

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
void GameClearScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    Box_->Draw(); // 背景の描画

    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();
    
    clear->Draw();// オブジェクトの描画


    player_->Draw();

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