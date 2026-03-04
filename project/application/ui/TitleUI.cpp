#include "TitleUI.h"
#include <TextureManager.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>

using namespace LoadResourceID;

void TitleUI::Initialize() {
	TextureManager::GetInstance()->LoadTexture(texture::W);
	Vector2 size = Vector2{ 40.0f,40.0f };
	ui_start_ = Sprite::Create(texture::W, Vector2{ 5.0f, 550.0f }, 0.0f, size);
}

void TitleUI::Update(){
	ui_start_->Update();
}

void TitleUI::Draw() {
	ui_start_->Draw();
}



