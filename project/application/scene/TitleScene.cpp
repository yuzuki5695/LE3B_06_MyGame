#include "TitleScene.h"
#include <SceneManager.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <CameraManager.h>
#include <Input.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <ParticleCommon.h>
#include <SkyboxCommon.h>
#include <FadeManager.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <StageManager.h>
#include <EditorEntityRegistry.h>
#include <Easing.h>
#include <UIManager.h>

using namespace Easing;
using namespace LoadResourceID;
namespace { constexpr float kFadeDuration = 1.0f; }

void TitleScene::Finalize() {
    FadeManager::GetInstance()->Finalize();  // フェードマネージャの解放処理
	StageManager::GetInstance()->Finalize(); // ステージマネージャの解放処理
	UIManager::GetInstance()->Finalize(); // UIマネージャの解放処理
}



void TitleScene::Initialize() {
#pragma region シーンの初期化  
    // カメラの初期化
    InitializeCamera();
    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();
    // リソースの読み込み
    LoadResources();
    // UIの初期化
    InitializeUI();
    // モデルの初期化
    InitializeModel();
    // パーティクルのの生成、初期化
    particle_ = std::make_unique<Titleparticle>();
    particle_->Initialize(player_->GetPlayerObject());
    StageManager::GetInstance()->Initialize();
    CameraManager::GetInstance()->SetCameraMode(CameraMode::Default);      
    // ImGuiエディタに情報を登録する
    EditorEntities();
    // UIマネージャの初期化
	UIManager::GetInstance()->Initialize();
#pragma endregion シーンの初期化
}

void TitleScene::InitializeCamera() {
    // カメラマネージャの生成、初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
}

void TitleScene::LoadResources() {
    //  テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture(texture::Ui02);
    TextureManager::GetInstance()->LoadTexture(texture::Title);
}

void TitleScene::InitializeUI() {}

void TitleScene::InitializeModel() {
    // プレイヤーパラメータ 
    playertransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.3f, 0.0f},  {-20.0f,0.0f,40.0f } };
    titleStartX_ = -20.0f;
    titleEndX_ = -10.0f;
    // モデル移動のパラメータ
    timer = 0.0f;
    moveDuration = kTitleMoveDuration;  // 移動にかけるフレーム数（約2秒）     
    moveFinished = false;
    time = 0.0f;          // 経過時間 
    // プレイヤーの生成、初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();
    player_->SetTransform(playertransform_);
    // 追従対象を設定
    CameraManager::GetInstance()->GetTitleCamera()->SetTarget(player_->GetPlayerObject());
   
    isPlayerZMoving_ = true;
	playerMoveDuration_ = 3.0f; // 2秒（60fpsなら120フレーム）
    playerMoveTimer_ = 0.0f;
	playerTargetZ_ = 700.0f; // 目標Z座標
    playerStartZ_ = playertransform_.translate.z;
}

void TitleScene::Update() {
    // シーン開始時にフェードインを開始
    UpdateFadeIn();
    // キー操作でのフェードアウトを開始
    UpdateFadeOut();
    // 次のシーンへ移行する
    UpdateSceneTransition();
    /*------------------------------------------*/
    /*--------------Cameraの更新処理--------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();
#pragma region 全てのObject3d個々の更新処理 
    StageManager::GetInstance()->Update();

    // プレイヤーの演出
    UpdateTitlePlayerMotion();
    player_->Update();
    ParticleManager::GetInstance()->Update(); 
    particle_->Update();
#pragma endregion 全てのObject3d個々の更新処理
    	
#pragma region 全てのSprite個々の更新処理

	// UIマネージャの更新
    UIManager::GetInstance()->Update();
#pragma endregion 全てのSprite個々の更新処理

#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI

    //FadeManager::GetInstance()->DrawImGui(); // フェードマネージャのImGui制御    
//    CameraManager::GetInstance()->DrawImGui();  // カメラマネージャのImGui制御
#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了
}

void TitleScene::Draw() {
#pragma region 全てのObject3d個々の描画処理 
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    StageManager::GetInstance()->DDSDraw();
    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();
    // プレイヤーの描画処理
    player_->Draw();
    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
    ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();
    // 各UIの描画処理
    UIManager::GetInstance()->Draw();
    // フェードの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}

void TitleScene::EditorEntities() {
    // 2D    
   // RegisterEditorEntity(ui_title_.get(), "Title");
    // 3D
    RegisterEditorEntity(player_->GetPlayerObject(), "Player");
}

void TitleScene::UpdateFadeIn() {
    // タイトルシーンにおけるフェード制御を担当
    FadeManager* fade = FadeManager::GetInstance();
    // フェードが未開始かつ実行中でない場合のみフェードインを開始
    if (!fade->IsFadeStart() && !fade->IsFading()) {
        fade->StartFadeIn(kFadeDuration, FadeStyle::SilhouetteSlide);
    }
    // フェード状態の更新
    fade->Update();
}

void TitleScene::UpdateFadeOut() {
    // タイトルシーンにおけるフェード制御を担当
    FadeManager* fade = FadeManager::GetInstance();
    if (Input::GetInstance()->Triggrkey(DIK_RETURN) && !fade->IsFading() && fade->IsFadeEnd() && !isPlayerBoost_ && !isPreFadeFollow_) {
        isPreFadeFollow_ = true;
        preFadeTimer_ = 0.0f;
        // 🔥 カメラをFollowに切り替え
        CameraManager::GetInstance()->GetTitleCamera()->SetTarget(player_->GetPlayerObject());
        CameraManager::GetInstance()->GetTitleCamera()->SetTitleCameraState(TitleCameraState::Follow);
        CameraManager::GetInstance()->SetCameraMode(CameraMode::Follow);
    }

    // ============================================
    // 1秒経過後にフェード開始
    // ============================================
    if (isPreFadeFollow_) {

        preFadeTimer_ += 1.0f / 60.0f; // deltaTime推奨

        if (preFadeTimer_ >= preFadeDuration_) {

            isPreFadeFollow_ = false;

            fade->StartFadeOut(2.5f, FadeStyle::SilhouetteExplode);

            // プレイヤーブースト開始
            isPlayerBoost_ = true;
            playerSpeedX_ = 0.0f;
        }
    }
}

void TitleScene::UpdateSceneTransition() {
    // タイトルシーンにおけるフェード制御を担当
    FadeManager* fade = FadeManager::GetInstance();
    // フェードアウト完了後にシーン遷移処理を行う
    if (fade->IsFadeEnd() && fade->GetFadeType() == FadeType::FadeOut) {
        // ゲームプレイシーンへ移行
        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

void TitleScene::UpdateTitlePlayerMotion() {
    FadeManager* fade = FadeManager::GetInstance();

    // ==============================
    // フェード中：加速して飛ぶ
    // ==============================
    if (isPlayerBoost_ && fade->IsFading() && fade->GetFadeType() == FadeType::FadeOut) {

        playerSpeedX_ += playerAccelX_;
        playerSpeedX_ = std::min(playerSpeedX_, playerMaxSpeedX_);

        playertransform_.translate.z += playerSpeedX_;

        // ちょっと上に浮かせると「飛び感」UP
        playertransform_.translate.y += 0.1f;

    }

    if (!isPlayerZMoving_ && fade->GetFadeType() != FadeType::FadeOut) {
        // ===== 上下の浮遊 =====
        time += kFloatSpeed;
        playertransform_.translate.y = sinf(time) * kFloatAmplitude;
    }
       
    if (isPlayerZMoving_) {

        playerMoveTimer_ += 1.0f / 60.0f; // deltaTimeがあるならそれ推奨

        float t = playerMoveTimer_ / playerMoveDuration_;
        t = std::clamp(t, 0.0f, 1.0f);

        // 🔥 好きなイージング
        float easeT = EaseOutCubic(t);

        float newZ = Lerp(playerStartZ_, playerTargetZ_, easeT);
        playertransform_.translate.z = newZ;

        if (t >= 1.0f) {
            isPlayerZMoving_ = false;
            playertransform_.translate.z = playerTargetZ_;
        }
    }
    
    if (!isPlayerZMoving_ && CameraManager::GetInstance()->GetMode() == CameraMode::Default) {
        CameraManager::GetInstance()->GetTitleCamera()->SetTitleCameraState(TitleCameraState::IntroMove);
    }

    player_->GetPlayerObject()->SetTranslate(playertransform_.translate);
}