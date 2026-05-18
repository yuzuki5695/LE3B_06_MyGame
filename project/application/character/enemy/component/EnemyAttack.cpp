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
        speed = 5.0f;
    }

    void EnemyAttack::Update(const Transform& transform, const Vector3& aimWorldPos) {
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
        Vector3 startPos = transform.translate;
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
}