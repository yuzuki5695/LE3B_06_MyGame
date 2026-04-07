#include "Player.h"
#include <ModelManager.h>
#include <Input.h>
#include <PlayerState.h>
#include <CameraManager.h>
#include <PlayerDataLoader.h>
#include <TextureManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace MyGame;
using namespace AssetGen::LoadResourceID::Models;
using namespace AssetGen::LoadResourceID::Textures;

Player::~Player() {}

void Player::Initialize() {
    // 3Dオブジェクト生成
    TextureManager::GetInstance()->LoadTexture(Ui::Target);
    ModelManager::GetInstance()->LoadModel(Character::Player);

    data_ = PlayerDataLoader::Load("player");

    object_ = Object3d::Create(Character::Player, data_.transform);
    targettransform_ = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 30.0f} };

    target_ = Object3d::Create(Character::Player, targettransform_);

    // レティクル初期化
    targetreticle_ = Sprite::Create(Ui::Target, Vector2{ 640.0f, 360.0f }, 0.0f, Vector2{ 100.0f, 100.0f });
    targetreticle_->SetTextureSize(Vector2{ 512.0f, 512.0f });
    targetreticle_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準

    // コンポーネントの生成
    move_ = std::make_unique<PlayerMove>();
    reticle_ = std::make_unique<PlayerReticle>();
    attack_ = std::make_unique<PlayerAttack>();
    // 初期ステートをセットする
    ChangeState(std::make_unique<PlayerStateIdle>());
}

void Player::Update() {
    // ステートの更新（現在のステートのUpdateが呼ばれる）
    state_.Update(*this);

    targetreticle_->Update();
    object_->Update();
}

void Player::Draw() {
    if (!isAlive_) { return; }

    if (object_) {
        object_->Draw();
    }
}

void Player::DrawSprite() {
    if (!isAlive_) { return; }
    if (targetreticle_) {
        targetreticle_->Draw();
    }
}