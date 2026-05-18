#include "GameOverUI.h"
#include <TextureManager.h>
#include <Easing.h>

// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Textures;
using namespace Easing;

namespace MyGame {

	void GameOverUI::Initialize() {

		// テクスチャの読み込み
		TextureManager::GetInstance()->LoadTexture(Ui::Gameover);
		TextureManager::GetInstance()->LoadTexture(Ui::UI_01);

		//// スプライトの生成、初期化
		//mission_ = Sprite::Create(Ui::MissionFailed, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 0.0f);


	}

	void GameOverUI::Update() {
		// スプライトの更新
		//mission_->Update();
	}

	void GameOverUI::Draw() {
		// スプライトの描画
		//mission_->Draw();
	}
}