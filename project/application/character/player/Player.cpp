#include "Player.h"
#include <ModelManager.h>

// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace MyGame;
using namespace AssetGen::LoadResourceID::Models;


Player::~Player() {}

void Player::Initialize() {
    // 3Dオブジェクト生成
    object_ = std::make_unique<MyEngine::Object3d>();

    ModelManager::GetInstance()->LoadModel(Character::Player);
    object_ = Object3d::Create(Character::Player, { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,10.0f} });


}

void Player::Update() {

    object_->Update();
}

void Player::Draw() {
    if (object_) {
        object_->Draw();
    }
}