#include "PlayerReticle.h"
#include <Input.h>
#include <MatrixVector.h>
#include <algorithm>
#include <MathUtil.h>
#include <CameraManager.h>
#include <Player.h>
#include <Enemy.h>

using namespace MyEngine;
using namespace MathUtil;
using namespace MatrixVector;

namespace MyGame {
    ///====================================================
    /// 更新処理
    ///====================================================
    void PlayerReticle::Update() {
        Input* input = Input::GetInstance();
        if (input->PushKey(DIK_LEFT)) {
            offset_.x -= kmovespeed_;
        }

        if (input->PushKey(DIK_RIGHT)) {
            offset_.x += kmovespeed_;
        }

        if (input->PushKey(DIK_UP)) {
            offset_.y += kmovespeed_;
        }

        if (input->PushKey(DIK_DOWN)) {
            offset_.y -= kmovespeed_;
        }

        offset_.x = std::clamp(offset_.x, -kMaxOffsetX, kMaxOffsetX);
        offset_.y = std::clamp(offset_.y, -kMaxOffsetY, kMaxOffsetY);
    }

    void PlayerReticle::UpdateLockOn(Player* player, const std::vector<std::unique_ptr<Enemy>>& enemies) {
        if (!player) {
            return;
        }

        // 現在使用中のカメラ
        Camera* camera = CameraManager::GetInstance()->GetActiveCamera();
        if (!camera) {
            return;
        }

        // レティクル(Target)の画面座標
        Vector2 reticleScreen = WorldToScreen(player->GetTarget()->GetTranslate(), camera);

        Enemy* bestEnemy = nullptr;
        float bestDistance = FLT_MAX;

        constexpr float kLockRange = 40.0f;   // ロック判定半径(px)
        for (const auto& enemy : enemies) {
            // 無効な敵は除外
            if (!enemy->IsAlive()) {
                continue;
            }
            if (!enemy->IsActive()) {
                continue;
            }
            // 敵の画面座標
            Vector2 enemyScreen = WorldToScreen(enemy->GetObject3d()->GetTranslate(), camera);

            // レティクルとの差
            float dx = enemyScreen.x - reticleScreen.x;
            float dy = enemyScreen.y - reticleScreen.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < kLockRange && distance < bestDistance) {
                bestDistance = distance;
                bestEnemy = enemy.get();
            }
        }

        // 一番近い敵をPlayerへ保存
        player->SetLockOnEnemy(bestEnemy);
    }

    Vector2 PlayerReticle::WorldToScreen(const Vector3& worldPos, Camera* camera) {
        if (!camera) {
            return {};
        }

        const Matrix4x4& vp = camera->GetViewProjectionMatrix();

        Vector4 clip = {
            worldPos.x * vp.m[0][0] +
            worldPos.y * vp.m[1][0] +
            worldPos.z * vp.m[2][0] +
            vp.m[3][0],

            worldPos.x * vp.m[0][1] +
            worldPos.y * vp.m[1][1] +
            worldPos.z * vp.m[2][1] +
            vp.m[3][1],

            worldPos.x * vp.m[0][2] +
            worldPos.y * vp.m[1][2] +
            worldPos.z * vp.m[2][2] +
            vp.m[3][2],

            worldPos.x * vp.m[0][3] +
            worldPos.y * vp.m[1][3] +
            worldPos.z * vp.m[2][3] +
            vp.m[3][3]
        };

        if (clip.w == 0.0f) {
            return {};
        }

        clip.x /= clip.w;
        clip.y /= clip.w;
        Vector2 screen;
        screen.x = (clip.x * 0.5f + 0.5f) * 1280.0f;
        screen.y = (-clip.y * 0.5f + 0.5f) * 720.0f;
        return screen;
    }
}