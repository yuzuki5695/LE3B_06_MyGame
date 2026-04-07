#include "PlayerAttack.h"
#include <Input.h>
#include <BulletManager.h>

using namespace MyEngine;

namespace MyGame {

    void PlayerAttack::Initialize() {
        timer_ = 0.0f;
    }

    void PlayerAttack::Update(const Transform& playerTransform, Object3d* target) {

        // =============================
        // クールタイム更新
        // =============================
        timer_ -= 1.0f / 60.0f;

        // =============================
        // 入力チェック
        // =============================
        if (Input::GetInstance()->Pushkey(DIK_SPACE) && timer_ <= 0.0f) {

            if (!target) return;

            // =============================
            // ① 発射方向計算
            // =============================
            Vector3 start = playerTransform.translate;
            Vector3 end = target->GetTransform().translate;

            Vector3 dir = end - start;

            // 正規化（重要）
            float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
            if (length > 0.0001f) {
                dir /= length;
            }

            // =============================
            // ② 速度設定
            // =============================
            float speed = 1.5f;
            Vector3 velocity = dir * speed;

            // =============================
            // ③ 弾生成
            // =============================
            BulletManager::GetInstance()->SpawnPlayerBullet(playerTransform, velocity);

            // =============================
            // ④ クールタイムリセット
            // =============================
            timer_ = coolTime_;
        }
    }
}