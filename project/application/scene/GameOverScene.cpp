#include "GameOverScene.h"
#include <CameraManager.h>
#include <SkyboxCommon.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
//#include <ParticleCommon.h>
#include <SceneName.h>
#include <FadeManager.h>
#include <UIManager.h>
#include <StageManager.h>
#include <ModelManager.h>
#include <random>
#include <SceneEmitterManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {
	///====================================================
    /// 終了処理
    ///====================================================
	void GameOverScene::Finalize() {
		CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
		FadeManager::GetInstance()->Finalize();   // フェードマネージャの終了処理
		UIManager::GetInstance()->Finalize();     // UIマネージャの終了処理 
		StageManager::GetInstance()->Finalize();  // ステージマネージャの終了処理
	}
    ///====================================================
    /// 初期化処理
    ///====================================================
	void GameOverScene::Initialize() {
		// カメラマネージャの初期化       
		CameraManager::GetInstance()->Initialize(SceneName::GAMEOVER);
		// モデルの読み込み    
		ModelManager::GetInstance()->LoadModel(Character::parts_01);
		ModelManager::GetInstance()->LoadModel(Character::parts_02);
		ModelManager::GetInstance()->LoadModel(Character::parts_03);
		ModelManager::GetInstance()->LoadModel(Character::parts_04);

		// ランダム生成
		std::random_device rd;
		std::mt19937 randomEngine(rd());
		std::uniform_real_distribution<float> transformY(15.0f, 20.0f);
		std::uniform_real_distribution<float> distX(-10.0f, 3.0f);
		std::uniform_real_distribution<float> distY(0.06f, 0.12f);
		std::uniform_real_distribution<float> rotateX(-0.01f, 0.01f);
		std::uniform_real_distribution<float> rotateY(-0.01f, 0.01f);
		std::uniform_real_distribution<float> rotateZ(-0.01f, 0.01f);


		std::vector<std::string> textureNames = { Character::parts_01,Character::parts_02,Character::parts_03,Character::parts_04, };
		partCount_ = 7;
		for (size_t i = 0; i < partCount_; ++i) {
			PartInfo part;

			part.transform = { {1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {distX(randomEngine), transformY(randomEngine), 30.0f} };
			// 1つ目はそのまま、それ以降は2個ずつテクスチャを切り替え
			size_t textureIndex = 0; // 1つ目は 0
			if (i > 0) {
				textureIndex = ((i - 1) / 2) + 1; // 2個ごとに切り替え
				if (textureIndex >= textureNames.size()) textureIndex = textureNames.size() - 1; // 配列オーバー防止
			}

			part.obj = Object3d::Create(Character::parts_01, part.transform);
			part.obj->SetModel(textureNames[textureIndex]); // 後からテクスチャを設定

			part.fallSpeed = { 0.0f, distY(randomEngine), 0.0f }; // Y軸落下
			part.rotateSpeed = { rotateX(randomEngine), rotateY(randomEngine), rotateZ(randomEngine) };

			partsList.push_back(std::move(part));
		}
		std::vector<Object3d*> partTargets;
		partTargets.reserve(partsList.size());  // 最適化

		for (auto& part : partsList) {
			partTargets.push_back(part.obj.get());  // unique_ptr → raw pointer に変換
		}
		// ステージマネージャの初期化
		StageManager::GetInstance()->Initialize();
		// フェードマネージャの初期化(フェードイン開始処理)
		FadeManager::GetInstance()->StartFade(FadeType::FadeIn, FadeStyle::SilhouetteExplode, 1.0f);
		// UIマネージャの初期化
		UIManager::GetInstance()->Initialize();
	}
	///====================================================
    /// 更新処理
	///====================================================
	void GameOverScene::Update() {
		// カメラマネージャの更新
		CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理			

		//  落下処理
		UpdateParts();
		// プレイヤ―パーツの更新
		for (PartInfo& part : partsList) {
			part.obj->Update();
		}

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
    ///====================================================
    /// 描画処理
    ///====================================================
	void GameOverScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
		// 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		SkyboxCommon::GetInstance()->Commondrawing();
		// ステージマネージャの描画
		StageManager::GetInstance()->DDSDraw();
		// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		Object3dCommon::GetInstance()->Commondrawing();

		// プレイヤ―パーツの描画
		for (PartInfo& part : partsList) {
			part.obj->Draw();
		}

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

	void GameOverScene::UpdateParts() {
		for (PartInfo& part : partsList) {
			// フェードインが完了するまでは落下させない
			if (!FadeManager::GetInstance()->IsFadeInFinished()) { return; }
			// 落下
			part.transform.translate.y -= part.fallSpeed.y;
			if (part.transform.translate.y < -20.0f) {
				part.transform.translate.y = -20.0f; // 地面で止める
			}
			// 回転
			part.transform.rotate -= part.rotateSpeed;
			// 更新反映
			part.obj->SetTranslate(part.transform.translate);
			part.obj->SetRotate(part.transform.rotate);

		}
	}
}