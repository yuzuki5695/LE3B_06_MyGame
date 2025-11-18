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

    offset_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.5f, 0.0f }, { -70.0f, 0.0f, 30.0f } };
    player_ = Object3d::Create("Gameplay/Model/Player/Player.obj", offset_); 
    startOffset_ = { -70.0f, 0.0f, 30.0f };  
    endOffset_ = { 0.0f, 0.0f, 30.0f };

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

    // フェードアウト完了後にタイトルへ
    if (FadeManager::GetInstance()->IsFadeEnd() && FadeManager::GetInstance()->GetFadeType() == FadeType::FadeOut) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/ 
    CameraManager::GetInstance()->SetTarget(player_.get());
    CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理    

	Box_->Update();   // 背景の更新
	clear->Update(); // オブジェクトの更新


    UpdateStep();
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


void GameClearScene::UpdateStep() {
    switch (step_)
    {
    case 0:
        Step1_MovePlayerAndSwitchCamera();
        break;

    case 1:
        Step2_WaitOrDoSomething();

        break;
    case 2:
        Step3_MoveCameraOnInput();
        break;
    }
}

void GameClearScene::Step1_MovePlayerAndSwitchCamera()
{
    // イージング処理
    bool finished = EaseMove(offset_.translate, startOffset_, endOffset_, easeT_, easeSpeed_);

    player_->SetTranslate(offset_.translate);

    if (finished)
    {
        // カメラモード変更（Follow → FixedLookAt）
        CameraManager::GetInstance()->GetClearCamera()->SetFollowMode(FollowMode::FixedLookAt);

        // 次のステップへ
        step_ = 1;
    }
}

void GameClearScene::Step2_WaitOrDoSomething()
{
    Camera* cam = CameraManager::GetInstance()->GetClearCamera()->GetActiveCamera();

    /*-----------------------------------------------*/
    /* ① カメラの X を 0 → 5 へイージング移動     */
    /*-----------------------------------------------*/
    if (step2CamT_ < 1.0f) {
        step2CamT_ += 0.01f;    // 速度
        if (step2CamT_ > 1.0f) step2CamT_ = 1.0f;

        float ease = EaseOutCubic(step2CamT_);
        float startX = 0.0f;
        float endX = 7.0f;

        Vector3 cp = cam->GetTranslate();
        cp.x = startX + (endX - startX) * ease;
        cam->SetTranslate(cp);
    }
    Vector3 pos = player_->GetTranslate();

    // プレイヤー揺れ or Y補完
    if (!step2FinishPlayerEase_) {
        step2Time_ += 0.03f;
        pos.y = sinf(step2Time_) * 2.3f;
        player_->SetTranslate(pos);
    } else {
        pos.y = Vector3::Lerp(pos, Vector3{ pos.x, 0.0f, pos.z }, 0.1f).y;
        player_->SetTranslate(pos);

        // 完全に0になったらStep3へ
        if (fabs(pos.y) < 0.01f) {
            step_ = 2;
        }
    }

    // ボタンでStep3に進める
    if (Input::GetInstance()->Triggrkey(DIK_RETURN) && step2CamT_ == 1.0f) {
        step2FinishPlayerEase_ = true;  // Y補完開始
    }
}

void GameClearScene::Step3_MoveCameraOnInput()
{
    Camera* cam = CameraManager::GetInstance()->GetClearCamera()->GetActiveCamera();

    // フリーモードに切り替え
    CameraManager::GetInstance()->GetClearCamera()->SetFollowMode(FollowMode::FreeOffset);

    // Step3中もプレイヤーのY座標は0に固定
    Vector3 playerPos = player_->GetTranslate();
    playerPos.x += 2.0f;
    player_->SetTranslate(playerPos);
 
  // 移動開始からのタイマー更新
    step3Timer_ += 1.0f / 60.0f; // 1フレーム想定 = 1/60秒

    // 1秒経過したら一瞬フェード処理
    if (!step3FadeTriggered_ && step3Timer_ >= 1.0f && !FadeManager::GetInstance()->IsFading() && FadeManager::GetInstance()->IsFadeEnd()) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::SilhouetteSlide); // 0.1秒だけの短いフェード
        step3FadeTriggered_ = true;
    }
}