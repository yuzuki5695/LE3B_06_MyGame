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
			// BaseCharacter状態からEnemyAlive状態へ遷移
            character.ChangeState(std::make_unique<EnemyAlive>());
        }
    }

    void EnemyAlive::Update(BaseCharacter& character) {        
        // 必要なコンポーネント
        Enemy* enemy = dynamic_cast<Enemy*>(&character);
        Player* player = dynamic_cast<Player*>(&character);
        
        // アクティブ中は各更新処理を行う
        if (character.IsActive()) {

			// 攻撃処理の更新処理
            enemy->GetAttack()->Update(enemy->GetObject3d()->GetTransform(), player->GetTranslate());

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