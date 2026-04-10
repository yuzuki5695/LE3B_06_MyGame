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

using namespace MyEngine;

namespace MyGame {

	void TitleScene::Finalize() {
		CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
		FadeManager::GetInstance()->Finalize();   // フェードマネージャの終了処理
	}

	void TitleScene::Initialize() {
		CameraManager::GetInstance()->Initialize(Transform({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -50.0f }));

		FadeManager::GetInstance()->StartFade(FadeType::FadeIn, FadeStyle::SilhouetteSlide, 1.0f);
	}

	void TitleScene::Update() {
#pragma region 全てのObject3d個々の更新処理			
		CameraManager::GetInstance()->Update();


#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理

		if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
			FadeManager::GetInstance()->SceneChangeFade(SceneName::GAMEPLAY, FadeStyle::SilhouetteExplode, 1.0f);
		}

		// フェードマネージャの更新
		FadeManager::GetInstance()->Update();

#pragma endregion 全てのSprite個々の更新処理
	}

	void TitleScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
		// 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		SkyboxCommon::GetInstance()->Commondrawing();

		// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		Object3dCommon::GetInstance()->Commondrawing();


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