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

namespace { constexpr float kFadeDuration = 1.0f; }

void TitleScene::Finalize() {
    FadeManager::GetInstance()->Finalize();  //  フェードマネージャの解放処理
}

void TitleScene::Initialize() {
#pragma region シーンの初期化  
    // カメラの初期化
    InitializeCamera();
    // リソースの読み込み
    LoadResources();
    // UIの初期化
    InitializeUI();

    // スカイボックス生成 
    skybox_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });
    // プレイヤーモデル生成（タイトル演出用） 
    playertransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.9f, 0.0f},  -20.0f,0.0f,40.0f };
    player_ = Object3d::Create("Title/Model/Player/Player.obj", playertransform_);
    titleStartX_ = -20.0f;
    titleEndX_ = -10.0f;
    // モデル移動のパラメータ
    timer = 0.0f;
    moveDuration = kTitleMoveDuration;  // 移動にかけるフレーム数（約2秒）     
    moveFinished = false;
    time = 0.0f;          // 経過時間 
    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();
    // パーティクルのの生成、初期化
    particle_ = std::make_unique<Titleparticle>();
    particle_->Initialize(player_.get());

#pragma endregion シーンの初期化
}

void TitleScene::InitializeCamera() {
    // カメラマネージャの生成、初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
}

void TitleScene::LoadResources() {
    //  テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture("Title/UI_02.png");
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
    // モデルの読み込み
    ModelManager::GetInstance()->LoadModel("Title/Title.obj");
    ModelManager::GetInstance()->LoadModel("Tile.obj");
    ModelManager::GetInstance()->LoadModel("Title/Model/Player/Player.obj");
}

void TitleScene::InitializeUI() {  
    // タイトルロゴの生成
    ui_title_ = Sprite::Create("Title/Title.png", Vector2{ 300.0f, 100.0f }, 0.0f, Vector2{ 600.0f,300.0f });
    ui_title_->SetTextureSize(Vector2{ 600.0f,300.0f });
    // スタートUIの生成
    ui_start_ = Sprite::Create("Title/UI_02.png", Vector2{ 420.0f, 500.0f }, 0.0f, Vector2{ 360.0f,90.0f });
    ui_start_->SetTextureSize(Vector2{ 360.0f,90.0f });
    // 更新・描画対象としてまとめる
    uiSprites_.clear();
    uiSprites_ = { ui_start_.get(),ui_title_.get() };
}

void TitleScene::Update() {
    // シーン開始時にフェードインを開始
    UpdateFadeIn();
    // キー操作でのフェードアウトを開始
    UpdateFadeOut();
    // 次のシーンへ移行する
    UpdateSceneTransition();

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();
#pragma region 全てのObject3d個々の更新処理 
    // Skyboxの回転
    Transform skyTrans = skybox_->GetTransform();
    skyTrans.rotate.y += 0.001f; // Y軸回転（1フレームごとに少しずつ）
    skybox_->SetRotate(skyTrans.rotate);
    skybox_->Update();
    // プレイヤーモデルの移動（
    if (!moveFinished) {
        // --- 位置のイージング ---
        timer += 1.0f;
        float t = std::clamp(timer / moveDuration, 0.0f, 1.0f);
 
        // EaseOutCubic
        float easeT = 1.0f - powf(1.0f - t, 3.0f);

        float startX = -20.0f;
        float endX   = -10.0f;

        playertransform_.translate.x = startX + (endX - startX) * easeT;

        // 移動が完了したらフラグを立てる
        if (t >= 1.0f) {
            moveFinished = true;
            timer = 0.0f;  // カラー用にリセット
        }

        player_->SetTranslate(playertransform_.translate);
    }


    time += kFloatSpeed;                     // 更新速度（0.05f は揺れの速さ） 
    playertransform_.translate.y = 0.0f + sinf(time) * kFloatAmplitude;  // 中心Y=-10.0f、振幅3.0f 
    player_->SetTranslate(playertransform_.translate);
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
    //// ImGuiでTileの位置・回転・スケールを変更
    //ImGui::Begin("Tile Controller");

    //ImGui::Text("Position");
    //ImGui::DragFloat3("Translation", &playertransform_.translate.x, 0.1f);

    //ImGui::Text("Rotation");
    //ImGui::DragFloat3("Rotation", &playertransform_.rotate.x, 0.1f);

    //
    //player_->SetTranslate(playertransform_.translate);
    //player_->SetRotate(playertransform_.rotate);

    //ImGui::End();

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

    player_->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
    ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();
	 
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
    if (Input::GetInstance()->Triggrkey(DIK_RETURN) && !fade->IsFading() && fade->IsFadeEnd()) {
        fade->StartFadeOut(kFadeDuration, FadeStyle::SilhouetteExplode);
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