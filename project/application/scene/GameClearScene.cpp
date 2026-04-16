#include "GameClearScene.h"
#include <CameraManager.h>
#include <SkyboxCommon.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <SceneName.h>

using namespace MyEngine;

namespace MyGame {
	void GameClearScene::Finalize() {
		CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
	}

	void GameClearScene::Initialize() {		
        CameraManager::GetInstance()->Initialize(SceneName::TITLE);

	}

	void GameClearScene::Update() {
#pragma region 全てのObject3d個々の更新処理			
		CameraManager::GetInstance()->Update();


#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理


#pragma endregion 全てのSprite個々の更新処理
	}

	void GameClearScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
		// 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		SkyboxCommon::GetInstance()->Commondrawing();

		// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		Object3dCommon::GetInstance()->Commondrawing();


		// パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
		ParticleCommon::GetInstance()->Commondrawing();

#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理 
		// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
		SpriteCommon::GetInstance()->Commondrawing();

#pragma endregion 全てのSprite個々の描画処理
	}
}