#include "EnemyState.h"
#include <BaseCharacter.h>
#include <Input.h>
#include <Player.h>
#include <CameraManager.h>
#include <Easing.h>
#include <Enemy.h>
#include <CollisionManager.h>

using namespace MyEngine;
using namespace Easing;

namespace MyGame {
    void EnemyIdle::Update(BaseCharacter& character) {
        timer_ += 1.0f / 60.0f;
        float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);
        float s = EaseOutBack(t);
        character.GetObject3d()->SetScale({ s, s, s });
        if (t >= 1.0f) {
            t = 1.0f;
            character.GetObject3d()->SetScale({ s, s, s });
            // 当たり判定登録
            character.RegisterCollider();
            character.ChangeState(std::make_unique<EnemyAlive>());
        }
    }

    void EnemyAlive::Update(BaseCharacter& character) {
		// アクティブ中は各更新処理を行う
        if (character.IsActive()) {


            // 毎フレーム当たり判定を更新
            character.GetCollider()->SetOBB(CollisionUtils::CreateOBB(character.GetObject3d()));
        }
    }

    void EnemyDead::Update(BaseCharacter& character) {
        if (character.IsAlive()) {
            // 死んだらフラグを立てる
            character.Destroy();
        }
    }
}