#include "TitleScene.h"
#include <SceneManager.h>
#include <SceneName.h>
#include <CameraManager.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <SkyboxCommon.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <Input.h>
#include <FadeManager.h>
#include <StageManager.h>
#include <Easing.h>
#include <UIManager.h>
#include <TitleUI.h>
#include <SceneEmitterManager.h>
#include <TitleParticle.h>
#include <SoundPlayer.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace Easing;
using namespace AssetGen::LoadResourceID;

namespace MyGame {
    ///====================================================
    /// 終了処理
    ///====================================================
	void TitleScene::Finalize() {
		CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
		FadeManager::GetInstance()->Finalize();   // フェードマネージャの終了処理
		StageManager::GetInstance()->Finalize();  // ステージマネージャの終了処理
		UIManager::GetInstance()->Finalize();     // UIマネージャの終了処理
		SceneEmitterManager::GetInstance()->Finalize(); // パーティクルエミッターマネージャの終了処理
		// オーディオの終了処理	
		SoundPlayer::GetInstance()->SoundUnload(&pushSE_);   // 決定音の解放
	}
    ///====================================================
    /// 初期化処理
    ///====================================================
	void TitleScene::Initialize() {
		CameraManager::GetInstance()->Initialize(SceneName::TITLE);

		// プレイヤ―の生成、初期化
		player_ = std::make_unique<Player>();
		player_->Initialize();
		playeroffset_ = { 0.0f,0.0f,-100.0f };
		player_->GetObject3d()->SetTranslate(playeroffset_);
		// カメラのターゲットにプレイヤーをセット
		CameraManager::GetInstance()->GetCurrentBehaviorAs<TitleCamera>()->SetTargetObject(player_->GetObject3d());

		// ステージマネージャの初期化
		StageManager::GetInstance()->Initialize();
		// UIマネージャの初期化
		UIManager::GetInstance()->Initialize();
		// フェードマネージャの初期化
		FadeManager::GetInstance()->StartFade(FadeType::FadeIn, FadeStyle::SilhouetteSlide, 1.0f);
		// パーティクルエミッターの初期化
		SceneEmitterManager::GetInstance()->Initialize();
		SceneEmitterManager::GetInstance()->GetEmitter<TitleParticle>()->SetObject3d(player_->GetObject3d());

		// オーディオの読み込み		
		pushSE_ = SoundLoader::SoundLoadWave(Audio::Bgsa);
	}
	///====================================================
    /// 更新処理
    ///====================================================
	void TitleScene::Update() {
#pragma region 全てのObject3d個々の更新処理				
		// カメラマネージャの更新
		CameraManager::GetInstance()->Update();
		if (Input::GetInstance()->TriggerKey(DIK_S)) {
			SoundPlayer::GetInstance()->SoundPlayWave(pushSE_, false, 0.2f);
		}

		// 移動処理
		PlayerMotion();
		
		player_->GetObject3d()->SetTranslate(playeroffset_);
		// プレイヤ―の更新
		player_->Update();
		
        // エミッターマネージャの更新
        SceneEmitterManager::GetInstance()->Update();
        // パーティクル更新
        ParticleManager::GetInstance()->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理		
		// UIマネージャの更新
		UIManager::GetInstance()->Update();

		// ステージマネージャの更新
		StageManager::GetInstance()->Update();
		// フェードマネージャの更新
		FadeManager::GetInstance()->Update();
#pragma endregion 全てのSprite個々の更新処理
	}
    ///====================================================
    /// 描画処理
    ///====================================================
	void TitleScene::Draw() {
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

	void TitleScene::PlayerMotion() {
		TitleUI* titleUI = UIManager::GetInstance()->GetUI<TitleUI>();
		
		if (isMoving_) {
			movetimer_ += 1.0f / 60.0f;
			float t = movetimer_ / moveDuration_;
			t = std::clamp(t, 0.0f, 1.0f);
			float easeT = EaseOutCubic(t);
			playeroffset_.z = Lerp(startZ_, targetZ_, easeT);
			// 到達
			if (t >= 1.0f) {
				playeroffset_.z = targetZ_;
				isMoving_ = false;
				// カメラ移動トリガー
				if (!isCameraTriggered_) {
					titleUI->Start();
					CameraManager::GetInstance()->GetCurrentBehaviorAs<TitleCamera>()->StartIntroMove();
					isCameraTriggered_ = true;
				}
			}
		}

		if (isCameraTriggered_) {
			floatTime_ += floatSpeed_ * (1.0f / 60.0f);
			playeroffset_.y = sinf(floatTime_) * floatAmplitude_;
		}

		// =========================
		// Enterで遷移開始
		// =========================
		if (Input::GetInstance()->TriggerKey(DIK_RETURN) && titleUI->IsFinished()) {
			titleUI->StartReverse();
		}

		if (titleUI->IsReverseFinished()) {
			isCameraTriggered_ = false;
			FadeManager::GetInstance()->SceneChangeFade(SceneName::GAMEPLAY, FadeStyle::SilhouetteExplode, 1.5f);
		}

		// =========================
		// 遷移中の動き（前進） 
		// =========================
		if (!isMoving_ && !isCameraTriggered_) {
			playeroffset_.z += 0.5f;
			playeroffset_.y += 0.1f;
		}
	}
}