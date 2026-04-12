#include "Enemy.h"
#include <ModelManager.h>
#include <EnemyState.h>
#include <CameraManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace MyGame;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {

    Enemy::~Enemy() {}
    
    void Enemy::Initialize() {
        ModelManager::GetInstance()->LoadModel(Character::Enemy);

        object_ = Object3d::Create(Character::Enemy, Transform{ { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        // 初期ステートをセットする
        ChangeState(std::make_unique<EnemyIdle>());
    }

    void Enemy::Update() {
        // ステートの更新（現在のステートのUpdateが呼ばれる）
        state_.Update(*this);

        object_->Update();
    }
    
    void Enemy::Draw() {
        if (!isAlive_) { return; }

        if (object_) {
            object_->Draw();
        }
    }
}