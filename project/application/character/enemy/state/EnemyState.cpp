#include "EnemyState.h"
#include <BaseCharacter.h>
#include <Input.h>
#include <Player.h>
#include <CameraManager.h>
#include <Easing.h>
#include <Enemy.h>
#include <CollisionManager.h>
#include <LineRenderer.h>

using namespace MyEngine;
using namespace Easing;
using namespace MatrixVector;

namespace MyGame {
    void EnemyIdle::Update(BaseCharacter& character) {
        // 必要なコンポーネント
        Enemy* enemy = dynamic_cast<Enemy*>(&character);
        // 敵が出現している場合は出現アニメーションを行う  
        if (enemy->IsSpawned()) {
            timer_ += 1.0f / 60.0f;
            float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);
            float s = EaseOutBack(t);
            character.GetObject3d()->SetScale({ s, s, s });
            if (t >= 1.0f) {
                t = 1.0f;
                character.GetObject3d()->SetScale({ s, s, s });
                // プレイヤー方向を向く   
                if (enemy->GetPlayer()) {
                    Vector3 dir = Normalize(enemy->GetPlayer()->GetTranslate() - character.GetObject3d()->GetTranslate());
                    float yaw = atan2f(dir.x, dir.z);
                    Transform transform = character.GetObject3d()->GetTransform();
                    transform.rotate.y = yaw;
                    character.GetObject3d()->SetRotate(transform.rotate);
                }
                // 出現完了したのでアクティブにする
                character.SetActive(true);
                // 出現フラグをオフにする
                enemy->SetSpawned(false);
                // 登録前にOBB更新
                character.GetCollider()->SetOBB(CollisionUtils::CreateOBB(character.GetObject3d()));
                character.GetCollider()->SetSize(enemy->GetColliderSize()); // サイズを上書き固定
                // 当たり判定登録
                character.RegisterCollider();
                // BaseCharacter状態からEnemyAlive状態へ遷移
                character.ChangeState(std::make_unique<EnemyAlive>());
            }
        }
    }

    void EnemyAlive::Update(BaseCharacter& character) {
        // 必要なコンポーネント
        Enemy* enemy = dynamic_cast<Enemy*>(&character);

        // アクティブ中またはプレイヤ―が非アクティブ中は各更新処理を行う
        if (character.IsActive() || !enemy->GetPlayer()->IsActive()) {
            
            if (enemy->HasPassedCamera()) {
                enemy->SetDeathType(Enemy::DeathType::System);
                character.ChangeState(std::make_unique<EnemyDead>());
                return;
            }

            // 攻撃処理の更新処理
            enemy->GetAttack()->Update(enemy->GetEnemyType(), enemy->GetObject3d(), enemy->GetPlayer()->GetTranslate());

            // 毎フレーム当たり判定を更新
            character.GetCollider()->SetOBB(CollisionUtils::CreateOBB(character.GetObject3d()));
        }
    }

    void EnemyDead::Update(BaseCharacter& character) {
        // 必要なコンポーネント
        Enemy* enemy = dynamic_cast<Enemy*>(&character);
        // ========================= 
        // システム死亡は即消し 
        // =========================
        if (enemy->GetDeathType() == Enemy::DeathType::System) {
            character.Destroy();
            return;
        } 
        // =========================
        // プレイヤーの弾による死亡のみ演出
        // =========================
        if (!enemy->IsDeathStarted()) {
            enemy->SetDeathStarted(true);
            // 当たり判定解除
            CollisionManager::GetInstance()->UnregisterCollider(character.GetCollider());
            character.SetActive(false); // アクティブフラグを下げる
            // 死亡演出処理の開始
            enemy->GetDeath()->Start();
        }

        // =====================
        // 死亡演出更新
        // =====================
        enemy->GetDeath()->Update(character.GetObject3d());
        // =====================
        // 演出終了
        // =====================
        if (enemy->GetDeath()->IsFinished()) {
            if (enemy->GetDeathType() == Enemy::DeathType::Player) {
                // パーティクルの処理をオンにする
                enemy->RequestDeathParticle();
                // 一度だけ伝えたいのでNoneに変える
                enemy->SetDeathType(Enemy::DeathType::None);
            }
            // 死んだらフラグを立てる
            character.Destroy();
        }
    }
}