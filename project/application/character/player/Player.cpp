#include "Player.h"
#include <ModelManager.h>
#include <Input.h>
#include <PlayerState.h>

// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace MyGame;
using namespace AssetGen::LoadResourceID::Models;

Player::~Player() {}

void Player::Initialize() {
    // 3Dオブジェクト生成
    ModelManager::GetInstance()->LoadModel(Character::Player);
    object_ = Object3d::Create(Character::Player, { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,10.0f} });

    // コンポーネントの生成
    moveComponent_ = std::make_unique<PlayerMove>();
    // 初期ステートをセットする
    ChangeState(std::make_unique<PlayerStateIdle>());
}

void Player::Update() {
    // ステートマシンの更新（現在のステートのUpdateが呼ばれる）
    state_.Update(*this);

    object_->Update();
}

void Player::Draw() {
    if (!isAlive_) { return; }

    if (object_) {
        object_->Draw();
    }
}