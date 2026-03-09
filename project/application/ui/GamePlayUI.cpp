#include "GamePlayUI.h"
#include <TextureManager.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>

using namespace LoadResourceID;

void GamePlayUI::Initialize() {
	TextureManager::GetInstance()->LoadTexture(texture::W);
	Vector2 size = Vector2{ 40.0f,40.0f };
	ui_start_ = Sprite::Create(texture::W, Vector2{ 640.0f, 550.0f }, 0.0f, size);
}

void GamePlayUI::Update(){
	ui_start_->Update();
}

void GamePlayUI::Draw() {
	ui_start_->Draw();
}



