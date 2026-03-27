#include "EnemyState.h"
#include <Enemy.h>
#include <Easing.h>
#include <CollisionConfig.h>

using namespace CollisionConfig;
using namespace Easing;

void EnemyStateWait::Update(Enemy* enemy) {
    // プレイヤーの座標を取得
    float playerZ = enemy->GetPlayer()->GetWorldPosition().z;
    // 自身の座標を取得
    float enemyZ = enemy->GetTranslate().z;

    // 差分が100未満になったらスポーン演出開始
    // ※ 敵が前方にいる前提（enemyZ - playerZ）
    if (enemyZ - playerZ < 200.0f) {
        enemy->ChangeState(std::make_unique<EnemyStateSpawn>());
    }
}

void EnemyStateSpawn::Enter(Enemy* enemy) {
    enemy->SetScale({ 0,0,0 });
}

void EnemyStateSpawn::Update(Enemy* enemy) {
    timer_ += 1.0f / 60.0f;
    float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);

    float s = EaseOutBack(t);
    enemy->SetScale({ s,s,s });

    if (t >= 1.0f) {
        enemy->ChangeState(std::make_unique<EnemyStateActive>());
    }
}

void EnemyStateActive::Update(Enemy* enemy) {
    // アクティブになったら初めて「敵」として認識させる
    enemy->SetCollisionAttribute(CollisionConfig::kGroupEnemy);

    // 誰と当たりたいかを「プレイヤーの弾」に設定
    enemy->SetCollisionMask(CollisionConfig::kGroupPlayerBullet);
    // 衝突マネージャに登録して、判定対象に含める
    CollisionManager::GetInstance()->RegisterCollider(enemy);

    enemy->GetWeapon()->Update(enemy->GetTranslate(), enemy->GetPlayer()->GetWorldPosition());
}

void EnemyStateDead::Update(Enemy* enemy) {
    // 衝突マネージャから登録を解除する
    CollisionManager::GetInstance()->UnregisterCollider(enemy);
    // 自身を削除対象にする
    enemy->SetDead(true);
}
