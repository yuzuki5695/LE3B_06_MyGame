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
    CameraManager::GetInstance()->Initialize(CameraTransform({ -15.0f, 15.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    CameraManager::GetInstance()->SetCameraMode(CameraMode::ClearCamera);
    // モデルの読み込み
    ModelManager::GetInstance()->LoadModel("Clear.obj");
    ModelManager::GetInstance()->LoadModel("Gameplay/Model/Player/Player.obj");

    // テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
    TextureManager::GetInstance()->LoadTexture("Gameclear/Texture/UI_01.png");
    TextureManager::GetInstance()->LoadTexture("Gameclear/Texture/Mission.png");
    TextureManager::GetInstance()->LoadTexture("Gameclear/Texture/Complete.png");



    Box_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });

    offset_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.5f, 0.0f }, { -70.0f, 0.0f, 30.0f } };
    player_ = Object3d::Create("Gameplay/Model/Player/Player.obj", offset_);
    startOffset_ = { -170.0f, 50.0f, 30.0f };
    endOffset_ = { 0.0f, 0.0f, 30.0f };

    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();


    ui1_ = Sprite::Create("Gameclear/Texture/UI_01.png", Vector2{ 950.0f, 450.0f }, 0.0f, Vector2{ 360.0f,100.0f });
    ui1_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準
    ui1_->SetTextureSize(Vector2{ 360.0f,100.0f });
        

    ui2Timer_ = 0.0f;
    ui2Duration_ = 60.0f;
    ui3Timer_ = 0.0f;
    ui3Duration_ = 60.0f;;
    ui2StartPos = { 1280.0f, 10.0f }; // 上にずらした初期位置
    ui2EndPos = { 600.0f, 10.0f }; // 最終位置（現在の値）
    ui3StartPos = { 1280.0f, 130.0f };
    ui3EndPos = { 800.0f, 130.0f };
    
    ui2_ = Sprite::Create("Gameclear/Texture/Mission.png", ui2StartPos, 0.0f, Vector2{ 400.0f,150.0f });
    ui2_->SetTextureSize(Vector2{ 400.0f,150.0f });

    ui3_ = Sprite::Create("Gameclear/Texture/Complete.png", ui3StartPos, 0.0f, Vector2{ 400.0f,150.0f });
    ui3_->SetTextureSize(Vector2{ 400.0f,150.0f });

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


    UpdateStep();
    player_->Update();

#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理

	ui1_->Update(); // スプライトの更新
    ui2_->Update();
    ui3_->Update();

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


    player_->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();

    if (ui2_->GetPosition().x == ui2EndPos.x && step_ == 1) {
        ui1_->Draw(); // スプライトの描画 
    }

    if (step_ != 0) {
        ui2_->Draw();
        ui3_->Draw();
    }

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
    Camera* cam = CameraManager::GetInstance()->GetClearCamera()->GetActiveCamera();
 
    if (!step1CamMoveStart_) {
        // 100進むと座標は -70
        if (offset_.translate.x > -80.0f) {
            step1CamMoveStart_ = true;
        }
    }

    if (step1CamMoveStart_ && step1CamT_ < 1.0f) {

        step1CamT_ += 0.02f; // カメラの速さ調整
        if (step1CamT_ > 1.0f) step1CamT_ = 1.0f;

        float e = EaseOutCubic(step1CamT_);

        float startX = -15.0f;
        float endX = 0.0f;

        float startY = 15.0f;
        float endY = 0.0f;

        Vector3 camPos = cam->GetTranslate();

        camPos.x = startX + (endX - startX) * e;
        camPos.y = startY + (endY - startY) * e;

        cam->SetTranslate(camPos);
    }

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
    /* ① カメラの X を 0 → 8 へイージング移動     */
    /*-----------------------------------------------*/
    if (step2CamT_ < 1.0f) {
        step2CamT_ += 0.01f;    // 速度
        if (step2CamT_ > 1.0f) step2CamT_ = 1.0f;

        float ease = EaseOutCubic(step2CamT_);
        float startX = 0.0f;
        float endX = 8.0f;

        Vector3 cp = cam->GetTranslate();
        cp.x = startX + (endX - startX) * ease;
        cam->SetTranslate(cp);
    }
    if (ui2Timer_ < ui2Duration_ && ui2_->GetPosition().x != ui2EndPos.x) {
        ui2Timer_++;

        float t = ui2Timer_ / ui2Duration_;
        float easeT = EaseOutBack(t);
        Vector2 newPos1 = {
            ui2StartPos.x + (ui2EndPos.x - ui2StartPos.x) * easeT,
            ui2StartPos.y + (ui2EndPos.y - ui2StartPos.y) * easeT
        };
        ui2_->SetPosition(newPos1);
    }

    if (ui3Timer_ < ui3Duration_ && ui2_->GetPosition().x != ui3EndPos.x && ui2_->GetPosition().x < 900) {
        ui3Timer_++;

        float t = ui3Timer_ / ui3Duration_;
        float easeT = EaseOutBack(t);
        Vector2 newPos1 = {
            ui3StartPos.x + (ui3EndPos.x - ui3StartPos.x) * easeT,
            ui3StartPos.y + (ui3EndPos.y - ui3StartPos.y) * easeT
        };
        ui3_->SetPosition(newPos1);
    }

    Vector3 pos = player_->GetTranslate();

    // プレイヤー揺れ or Y補完
    if (!step2FinishPlayerEase_) {
        step2Time_ += 0.03f;
        pos.y = sinf(step2Time_) * 1.3f;
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
        if (!step2FinishPlayerEase_) {
            ui2Timer_ = 0;
            ui3Timer_ = 0;
            ui2StartPos = { 600.0f, 10.0f }; // 上にずらした初期位置 
            ui2EndPos = { 1280.0f, 10.0f }; // 最終位置（現在の値）
            ui3StartPos = { 800.0f, 130.0f };
            ui3EndPos = { 2480.0f, 130.0f };
        }
        step2FinishPlayerEase_ = true;  // Y補完開始
    }
}

void GameClearScene::Step3_MoveCameraOnInput()
{
    Camera* cam = CameraManager::GetInstance()->GetClearCamera()->GetActiveCamera();
    CameraManager::GetInstance()->GetClearCamera()->SetFollowMode(FollowMode::FreeOffset);

    if (ui2Timer_ < ui2Duration_ && ui2_->GetPosition().x != ui2EndPos.x) {
        ui2Timer_++; 

        float t = ui2Timer_ / ui2Duration_;
        float easeT = EaseOutBack(t);
        Vector2 newPos1 = {
            ui2StartPos.x + (ui2EndPos.x - ui2StartPos.x) * easeT,
            ui2StartPos.y + (ui2EndPos.y - ui2StartPos.y) * easeT
        };
        ui2_->SetPosition(newPos1);
    }

    if (ui3Timer_ < ui3Duration_ && ui2_->GetPosition().x != ui3EndPos.x && ui2_->GetPosition().x > 900) {
        ui3Timer_++;

        float t = ui3Timer_ / ui3Duration_;
        float easeT = EaseOutBack(t);
        Vector2 newPos1 = {
            ui3StartPos.x + (ui3EndPos.x - ui3StartPos.x) * easeT,
            ui3StartPos.y + (ui3EndPos.y - ui3StartPos.y) * easeT
        };
        ui3_->SetPosition(newPos1);
    }


    // Step3時間更新
    step3Timer_ += (1.0f / 60.0f);

    Vector3 pos = player_->GetTranslate();

    /*-----------------------------------------------*/
    /* ① Step3開始時の「一度だけ後ろに下がる動き」 */
    /*-----------------------------------------------*/
    if (!step3BackJumpDone_)
    {
        float backDuration = 0.3f;        // 0.5秒かけて後ろへ
        float backDistance = -15.0f;      // 大きく下がる距離

        float t = step3Timer_ / backDuration;
        if (t > 1.0f) {
            t = 1.0f;
            step3BackJumpDone_ = true;
        }

        float ease = EaseOutBack(t);

        pos.x = startStep3PosX_ + backDistance * ease;
        pos.y = 0.0f;
        // --- 回転：1.5 → 3.0 へ ---
        Vector3 rot = player_->GetRotate();
        float startRot = 1.5f;
        float endRot = 3.0f;

        rot.y = startRot + (endRot - startRot) * ease;  // ← イージング回転

        // プレイヤーに設定
        player_->SetTranslate(pos);
        player_->SetRotate(rot);

        return;
    }

    /*-----------------------------------------------*/
    /* ② カメラへ向かって X/Y/Z 移動（加速付き）   */
    /*-----------------------------------------------*/

    // Z の進捗 (30 → 0)
    float t = (30.0f - pos.z) / 30.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    // 加速する速度
    float baseSpeed = 1.0f;
    float accelPower = 6.0f;
    float speed = baseSpeed + EaseOutCubic(t) * accelPower;

    // Z を前へ
    pos.z -= speed;

    /*-----------------------------------------------*/
    // 追加：X に横方向のカーブを入れる
    /*-----------------------------------------------*/

    // X の最大曲がり幅
    float maxCurve = 25.0f;     // ← 好きな値にする（大きいほど横移動が強い）

    // 進行に合わせてカーブが減る（最初大きく→最後0へ）
    float curveX = EaseOutCubic(1.0f - t) * maxCurve;

    /*-----------------------------------------------*/
    // 斜め上へ（Yは上昇）＋ カーブX追加
    /*-----------------------------------------------*/
    float rise = EaseOutCubic(t) * 15.0f;

    Vector3 targetPos = { curveX,rise,pos.z };

    // X/Y をなめらかに寄せる
    pos = Vector3::Lerp(pos, targetPos, 0.12f);

    player_->SetTranslate(pos);



    /*-----------------------------------------------*/
    /* ③ 1秒後に一瞬フェード処理                  */
    /*-----------------------------------------------*/
    if (!step3FadeTriggered_ &&
        step3Timer_ >= 1.0f &&
        !FadeManager::GetInstance()->IsFading() &&
        FadeManager::GetInstance()->IsFadeEnd())
    {
        FadeManager::GetInstance()->StartFadeOut(1.3f, FadeStyle::SilhouetteSlide);
        step3FadeTriggered_ = true;
    }
}