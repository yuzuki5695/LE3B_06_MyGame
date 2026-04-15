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

using namespace MyEngine;

namespace MyGame {

	void TitleScene::Finalize() {
		CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
		FadeManager::GetInstance()->Finalize();   // フェードマネージャの終了処理
		StageManager::GetInstance()->Finalize();  // ステージマネージャの終了処理
	}

	void TitleScene::Initialize() {
		CameraManager::GetInstance()->Initialize(Transform({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));

		// プレイヤ―の生成、初期化
		player_ = std::make_unique<Player>();
		player_->Initialize();
		CameraManager::GetInstance()->SetTarget(player_->GetObject3d());
		playeroffset_ = { 0.0f,0.0f,-100.0f };	
		player_->GetObject3d()->SetTranslate(playeroffset_);

		// ステージマネージャの初期化
		StageManager::GetInstance()->Initialize();
		// フェードマネージャの初期化
		FadeManager::GetInstance()->StartFade(FadeType::FadeIn, FadeStyle::SilhouetteSlide, 1.0f);
	}

	void TitleScene::Update() {


		if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
			//FadeManager::GetInstance()->SceneChangeFade(SceneName::GAMEPLAY, FadeStyle::SilhouetteExplode, 1.0f);
		}

#pragma region 全てのObject3d個々の更新処理				
		// カメラマネージャの更新
		CameraManager::GetInstance()->Update();

		// 移動処理
		if (isMoving_) {
			playeroffset_.z += 0.5f; // 速度（調整OK）

			if (playeroffset_.z >= 100.0f) {
				playeroffset_.z = 100.0f;
				isMoving_ = false; // 停止
			}
		}

		player_->GetObject3d()->SetTranslate(playeroffset_);
		// プレイヤ―の更新
		player_->Update();

#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理


		// ステージマネージャの更新
		StageManager::GetInstance()->Update();
		// フェードマネージャの更新
		FadeManager::GetInstance()->Update();
#pragma endregion 全てのSprite個々の更新処理
	}

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
		//ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理 
		// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
		SpriteCommon::GetInstance()->Commondrawing();
		// フェードの描画
		FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
	}
}