#include "GameOverScene.h"
#include <CameraManager.h>
#include <SkyboxCommon.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <SceneName.h>
#include <FadeManager.h>
#include <UIManager.h>
#include <StageManager.h>

using namespace MyEngine;

namespace MyGame {
	void GameOverScene::Finalize() {
		CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
		FadeManager::GetInstance()->Finalize();   // フェードマネージャの終了処理
		UIManager::GetInstance()->Finalize();     // UIマネージャの終了処理 
		StageManager::GetInstance()->Finalize();  // ステージマネージャの終了処理
	}

	void GameOverScene::Initialize() {
		// カメラマネージャの初期化       
		CameraManager::GetInstance()->Initialize(SceneName::GAMEOVER);


		// ステージマネージャの初期化
		StageManager::GetInstance()->Initialize();
		// フェードマネージャの初期化(フェードイン開始処理)
		FadeManager::GetInstance()->StartFade(FadeType::FadeIn, FadeStyle::SilhouetteExplode, 1.0f);
		// UIマネージャの初期化
		UIManager::GetInstance()->Initialize();
	}

	void GameOverScene::Update() {
		// カメラマネージャの更新
		CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理			


		// ステージマネージャの更新
        StageManager::GetInstance()->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理
		// UIマネージャの更新
        UIManager::GetInstance()->Update();
        // フェードマネージャの更新
        FadeManager::GetInstance()->Update();
#pragma endregion 全てのSprite個々の更新処理
	}

	void GameOverScene::Draw() {
	#pragma region 全てのObject3d個々の描画処理
		// 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		SkyboxCommon::GetInstance()->Commondrawing();
		// ステージマネージャの描画
        StageManager::GetInstance()->DDSDraw();
		// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		Object3dCommon::GetInstance()->Commondrawing();

		// ステージマネージャの描画
        StageManager::GetInstance()->Draw();
		// パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
		ParticleCommon::GetInstance()->Commondrawing();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理 
		// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
		SpriteCommon::GetInstance()->Commondrawing();
	
		// UIマネージャの描画
        UIManager::GetInstance()->Draw();
        // フェードマネージャの描画
        FadeManager::GetInstance()->Draw();

#pragma endregion 全てのSprite個々の描画処理
	}
}