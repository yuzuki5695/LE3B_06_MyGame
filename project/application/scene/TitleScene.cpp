#include "TitleScene.h"
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
#include<Tools/AssetGenerator/engine/math/LoadResourceID.h>

using namespace LoadResourceID;
namespace { constexpr float kFadeDuration = 1.0f; }

void TitleScene::Finalize() {
    FadeManager::GetInstance()->Finalize();  //  フェードマネージャの解放処理
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
        
    CameraManager::GetInstance()->SetCameraMode(CameraMode::Default);      

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
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
}

void TitleScene::InitializeUI() {  
    // タイトルロゴの生成
    ui_title_ = Sprite::Create(texture::Title, Vector2{300.0f, 100.0f}, 0.0f, Vector2{600.0f,300.0f});
    ui_title_->SetTextureSize(Vector2{ 600.0f,300.0f });
    // スタートUIの生成
    ui_start_ = Sprite::Create(texture::Ui02, Vector2{ 420.0f, 500.0f }, 0.0f, Vector2{ 360.0f,90.0f });
    ui_start_->SetTextureSize(Vector2{ 360.0f,90.0f });
    // 更新・描画対象としてまとめる
    uiSprites_.clear();
    uiSprites_ = { ui_start_.get(),ui_title_.get() };
}

void TitleScene::InitializeModel() {
    // スカイボックス生成 
    skybox_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });
    // プレイヤーパラメータ 
    playertransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.3f, 0.0f},  -20.0f,0.0f,40.0f };
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
    // Skyboxの回転
    Transform skyTrans = skybox_->GetTransform();
    skyTrans.rotate.y += 0.001f; // Y軸回転（1フレームごとに少しずつ）
    skybox_->SetRotate(skyTrans.rotate);
    skybox_->Update();
     
    // プレイヤーの演出
    UpdateTitlePlayerMotion();
    player_->Update();

    ParticleManager::GetInstance()->Update(); 
    particle_->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理
    for (Sprite* sprite : uiSprites_) {
        sprite->Update();
    }
#pragma endregion 全てのSprite個々の更新処理

#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI
	FadeManager::GetInstance()->DrawImGui(); // フェードマネージャのImGui制御    
    CameraManager::GetInstance()->DrawImGui();  // カメラマネージャのImGui制御
#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了
}

void TitleScene::Draw() {
#pragma region 全てのObject3d個々の描画処理 
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    skybox_->Draw();
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
    for (Sprite* sprite : uiSprites_) {
        sprite->Draw();
    }
	// フェードの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
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
    // フェード中ではなくフェード完了状態の時にEnterキーを押すとフェードアウト開始
    if (Input::GetInstance()->Triggrkey(DIK_RETURN) && !fade->IsFading() && fade->IsFadeEnd() && !isPlayerBoost_ && CameraManager::GetInstance()->GetActiveCamera()->GetTranslate().x >= 30.0f) {
        fade->StartFadeOut(2.5f, FadeStyle::SilhouetteExplode);
        CameraManager::GetInstance()->SetCameraMode(CameraMode::Follow);

        // ===== プレイヤー演出開始 =====
        isPlayerBoost_ = true;
        playerSpeedX_ = 0.0f;
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
        playertransform_.translate.y += 0.05f;

    }

    // ===== 横移動（イージング） =====
    if (!moveFinished) {
        timer += 1.0f;
        float t = std::clamp(timer / moveDuration, 0.0f, 1.0f);

        // EaseOutCubic
        float easeT = 1.0f - powf(1.0f - t, 3.0f);

        playertransform_.translate.x =
            titleStartX_ + (titleEndX_ - titleStartX_) * easeT;

        if (t >= 1.0f) {
            moveFinished = true;
            timer = 0.0f;
        }
    }
    if (fade->GetFadeType() != FadeType::FadeOut) {
        // ===== 上下の浮遊 =====
        time += kFloatSpeed;
        playertransform_.translate.y = sinf(time) * kFloatAmplitude;
    }

    // ===== Transform を Object に反映 =====
    player_->GetPlayerObject()->SetTranslate(playertransform_.translate);
}