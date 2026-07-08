#include "GameClearScene.h"
#include <CameraManager.h>
#include <SkyboxCommon.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <SceneName.h>
#include <StageManager.h>
#include <FadeManager.h>
#include <UIManager.h>
#include <Easing.h>
#include <GameClearUI.h>
#include <Input.h>
#include <SceneEmitterManager.h>
#include <GameClearParticle.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace MyEngine;
using namespace Easing;

namespace MyGame {
    void GameClearScene::Finalize() {
        CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
        FadeManager::GetInstance()->Finalize();   // フェードマネージャの終了処理
        UIManager::GetInstance()->Finalize();     // UIマネージャの終了処理 
        StageManager::GetInstance()->Finalize();  // ステージマネージャの終了処理
        SceneEmitterManager::GetInstance()->Finalize(); // パーティクルエミッターマネージャの終了処理
    }

    void GameClearScene::Initialize() {
        CameraManager::GetInstance()->Initialize(SceneName::GAMECLEAR);

        offset_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.5f, 0.0f }, { -170.0f, 50.0f, 20.0f } };
        startOffset_ = { -170.0f, 50.0f, 20.0f };
        endOffset_ = { 0.0f, 0.0f, 20.0f };
        // プレイヤ―の生成、初期化
        player_ = std::make_unique<Player>();
        player_->Initialize();
        player_->SetRotate(offset_.rotate);
        player_->SetTranslate(offset_.translate);

        // カメラのターゲットにプレイヤーをセット
        CameraManager::GetInstance()->GetCurrentBehaviorAs<GameClearCamera>()->SetTargetObject(player_->GetObject3d());
        // 演出の段階を初期化
        step_ = 0;

        // ステージマネージャの初期化
        StageManager::GetInstance()->Initialize();
        // UIマネージャの初期化
        UIManager::GetInstance()->Initialize();
        // フェードマネージャの初期化
        FadeManager::GetInstance()->StartFade(FadeType::FadeIn, FadeStyle::Normal, 1.0f);
        // パーティクルエミッターの初期化
        SceneEmitterManager::GetInstance()->Initialize();
        SceneEmitterManager::GetInstance()->GetEmitter<GameClearParticle>()->SetPlayer(player_.get());
    }

    void GameClearScene::Update() {
        // カメラマネージャの更新
        CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理

        UpdateStep();

        // プレイヤ―の更新
        player_->Update();

        // ステージマネージャの更新
        StageManager::GetInstance()->Update();
        // エミッターマネージャの更新
        SceneEmitterManager::GetInstance()->Update();
        // パーティクル更新
        ParticleManager::GetInstance()->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理
        // UIマネージャの更新
        UIManager::GetInstance()->Update();
        // フェードマネージャの更新
        FadeManager::GetInstance()->Update();
#pragma endregion 全てのSprite個々の更新処理
    }

    void GameClearScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
        // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
        SkyboxCommon::GetInstance()->Commondrawing();
        // ステージマネージャの描画
        StageManager::GetInstance()->DDSDraw();
        // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
        Object3dCommon::GetInstance()->Commondrawing();

        // プレイヤ―の描画
        player_->Draw();

        // ステージマネージャの描画
        StageManager::GetInstance()->Draw();
        // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
		ParticleCommon::GetInstance()->Commondrawing();
		ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理 
        // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
        SpriteCommon::GetInstance()->Commondrawing();
        // UIマネージャの描画
        UIManager::GetInstance()->Draw();
        // フェードの描画
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

    void GameClearScene::Step1_MovePlayerAndSwitchCamera() {

        // イージング処理
        bool finished = EaseMove(offset_.translate, startOffset_, endOffset_, easeT_, easeSpeed_);

        if (finished) {
            // 次のステップへ
            step_ = 1;
            // カメラの状態を移動モードに切り替える
            CameraManager::GetInstance()->GetCurrentBehaviorAs<GameClearCamera>()->SetCameraState(CameraDefs::CameraState::Move);
            UIManager::GetInstance()->GetUI<GameClearUI>()->StartMove();
        }
        // プレイヤーの位置を更新
        player_->SetTranslate(offset_.translate);
    }
    
    bool GameClearScene::EaseMove(Vector3& current, const Vector3& start, const Vector3& end, float& t, float speed) {
        if (t >= 1.0f) return true;
        t = std::min(1.0f, t + speed);
        current = Easing::EaseVector3(start, end, t, Easing::EaseOutSine);
        return (t >= 1.0f);
    }

    void GameClearScene::Step2_WaitOrDoSomething() {
        // プレイヤー揺れ or Y補完
        if (!step2FinishPlayerEase_) {
            step2Time_ += 0.03f;
            offset_.translate.y = sinf(step2Time_) * 1.3f;
            player_->SetTranslate(offset_.translate);
        } else {
            offset_.translate.y = Vector3::Lerp(offset_.translate, Vector3{ offset_.translate.x, 0.0f, offset_.translate.z }, 0.1f).y;
            player_->SetTranslate(offset_.translate);
            // 完全に0になったらStep3へ
            if (fabs(offset_.translate.y) < 0.01f) {
                step_ = 2;
                FadeManager::GetInstance()->SceneChangeFade(SceneName::TITLE, FadeStyle::SilhouetteExplode, 1.0f);
            }
        }

        // ボタンでStep3に進める
        if (Input::GetInstance()->TriggerKey(DIK_RETURN) && UIManager::GetInstance()->GetUI<GameClearUI>()->GetUI01Finished()) {
            if (!step2FinishPlayerEase_) {
                UIManager::GetInstance()->GetUI<GameClearUI>()->UI01Finished();
                UIManager::GetInstance()->GetUI<GameClearUI>()->StartBack();
                UIManager::GetInstance()->GetUI<GameClearUI>()->SetMovestarted(false);
            }
            step2FinishPlayerEase_ = true;  // Y補完開始
        }
    }

    void GameClearScene::Step3_MoveCameraOnInput() {

        CameraManager::GetInstance()->GetCurrentBehaviorAs<GameClearCamera>()->SetCameraState(CameraDefs::CameraState::LockOn);

        // Step3時間更新
        step3Timer_ += (1.0f / 60.0f) * step3TimeScale;

        Vector3 pos = player_->GetTranslate();

        /*-----------------------------------------------*/
        /* ① Step3開始時の「一度だけ後ろに下がる動き」 */
        /*-----------------------------------------------*/
        if (!step3BackJumpDone_) {
            float backDuration = 0.3f;
            float backDistance = -30.0f;   // 後ろ（-Z）

            float t = step3Timer_ / backDuration;
            if (t > 1.0f) {
                t = 1.0f;
                step3BackJumpDone_ = true;
                // この時点のZを基準にする
                startStep3PosZ_ = pos.z;
            }

            float ease = EaseOutBack(t);

            // Zのみ後退
            pos.z = startStep3PosZ_ + backDistance * ease;
            pos.y = 0.0f;

            // 回転のみ変更
            Vector3 rot = player_->GetRotate();
            float startRot = 1.5f;
            float endRot = 3.0f;
            rot.y = startRot + (endRot - startRot) * ease;

            player_->SetTranslate(pos);
            player_->SetRotate(rot);
            return;
        }
    }

}