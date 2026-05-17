#include "EnemyAttack.h"
#include <Input.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include <CameraManager.h>

using namespace MyEngine;
using namespace MatrixVector;

namespace MyGame {

    void EnemyAttack::Initialize() {
        timer_ = 0.0f;
        interval_ = 1.0f; // 1秒ごとに発射
    }

    void EnemyAttack::Update(const Transform& transform, const Vector3& aimWorldPos) {

        // =========================
        // タイマー更新
        // =========================
        timer_ += 1.0f / 60.0f;

        if (timer_ < interval_) {
            return;
        }

        timer_ = 0.0f;

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
        float speed = 0.5f;
        Vector3 velocity = dir * speed;

        // =========================
        // 弾生成
        // =========================
        Transform bulletTransform;
        bulletTransform.translate = startPos;

        BulletManager::GetInstance()->SpawnEnemyBullet(bulletTransform, velocity);
    }
}