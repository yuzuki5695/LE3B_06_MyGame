#include "EnemyAttack.h"
#include <Input.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include <CameraManager.h>

using namespace MyEngine;
using namespace MatrixVector;

namespace MyGame {

    void EnemyAttack::Initialize() {
        // 初期化
        // 乱数エンジン初期化
        randomEngine_ = std::mt19937(rd());
        distcooltime_ = std::uniform_real_distribution<float>(2.0f, 5.0f); // クールタイムを設定
        coolTime_ = distcooltime_(randomEngine_); // 最初のクールタイムをランダムに決定
        canShoot_ = false;    // 最初は撃てないようにする
        // 最初は即撃てるようにする
        timer_ = coolTime_;
        speed = 3.0f;
        state_ = AttackState::Idle;
    }

    void EnemyAttack::Update(EnemyType type, Object3d* object, const Vector3& aimWorldPos) {

        switch (type) {

        case EnemyType::Shot: // 弾を撃つ
            UpdateShot(object, aimWorldPos);
            break;
        case EnemyType::Tackle: // 突撃する
            UpdateTackle(object, aimWorldPos);
            break;
        }
    }

    void EnemyAttack::UpdateShot(Object3d* object, const Vector3& aimWorldPos) {
        // =========================
        // タイマー更新
        // =========================
        timer_ += 1.0f / 60.0f;
        if (timer_ < coolTime_) {
            return;
        }

        // 発射したのでタイマーリセット
        timer_ = 0.0f;

        // 次回クールタイムをランダム決定 
        coolTime_ = distcooltime_(randomEngine_);

        // =========================
        // 発射位置（敵の位置）
        // =========================
        Vector3 startPos = object->GetTransform().translate;
        // =========================
        // プレイヤー方向
        // =========================
        Vector3 dir = aimWorldPos - startPos;
        dir = Normalize(dir);
        // =========================
        // 速度
        // =========================
        Vector3 velocity = dir * speed;
        // =========================
        // 弾生成
        // =========================
        Transform bulletTransform;
        bulletTransform.scale = { 1.0f,1.0f,1.0f };
        bulletTransform.rotate = { 0.0f,0.0f,0.0f };
        bulletTransform.translate = startPos;

        // 弾生成
        BulletManager::GetInstance()->SpawnEnemyBullet(bulletTransform, velocity);

        // 発射後は撃てないようにする
        canShoot_ = false;
    }

    void EnemyAttack::UpdateTackle(Object3d* object, const Vector3& aimWorldPos) {
        Transform transform = object->GetTransform();

        switch (state_) {
        case AttackState::Idle: {
            timer_ += 1.0f / 60.0f;

            if (timer_ >= coolTime_) {
                timer_ = 0.0f;
                coolTime_ = distcooltime_(randomEngine_);
                // プレイヤー方向固定
                Vector3 dir = Normalize(aimWorldPos - transform.translate);
                dashVelocity_ = dir * dashSpeed_;
                dashDistance_ = 0.0f;
                state_ = AttackState::Dash;
            }

            break;
        }
        case AttackState::Dash: {
            transform.translate += dashVelocity_;
            transform.rotate.z += dashRotationSpeed_;

            object->SetTranslate(transform.translate);
            object->SetRotate(transform.rotate);
            break;
        }
        }
    }
    
}