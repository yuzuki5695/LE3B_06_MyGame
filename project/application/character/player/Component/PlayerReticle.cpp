#include "PlayerReticle.h"
#include <Input.h>
#include <MatrixVector.h>
#include <algorithm>
#include <MathUtil.h>

using namespace MyEngine;
using namespace MathUtil;
using namespace MatrixVector;

namespace MyGame {
    void PlayerReticle::UpdateInput() {
        Input* input = Input::GetInstance();

        Vector2 move = { 0.0f, 0.0f };

        // ← → ↑ ↓
        if (input->PushKey(DIK_LEFT))  move.x -= 1.0f;
        if (input->PushKey(DIK_RIGHT)) move.x += 1.0f;
        if (input->PushKey(DIK_UP))    move.y -= 1.0f;
        if (input->PushKey(DIK_DOWN))  move.y += 1.0f;

        // 正規化（斜め移動対策）
        if (move.x != 0.0f || move.y != 0.0f) {
            float len = sqrtf(move.x * move.x + move.y * move.y);
            move.x /= len;
            move.y /= len;
        }

        // 速度適用
        screenPos_.x += move.x * moveSpeed_;
        screenPos_.y += move.y * moveSpeed_;

        // 画面外制限
        screenPos_.x = std::clamp(screenPos_.x, 0.0f, 1280.0f);
        screenPos_.y = std::clamp(screenPos_.y, 0.0f, 720.0f);
    }

    Vector3 PlayerReticle::ScreenToWorld(const Vector2& screenPos, Camera* camera) {
        if (!camera) {
            return {};
        }

        // ----------------------------
        // スクリーン中心基準
        // ----------------------------
        float offsetX = screenPos.x - 640.0f;
        float offsetY = screenPos.y - 360.0f;

        // 感度
        constexpr float scale = 0.08f;
        // カメラ回転
        Vector3 camRot = camera->GetRotate();

        float yaw = camRot.y;
        float pitch = camRot.x;

        // カメラ基底
        Vector3 forward = { sinf(yaw) * cosf(pitch),-sinf(pitch),cosf(yaw) * cosf(pitch) };

        forward = Normalize(forward);

        Vector3 worldUp = { 0,1,0 };
        Vector3 right = Normalize(Cross(worldUp, forward));
        Vector3 up = Normalize(Cross(forward, right));

        // ----------------------------
        // レティクルの
        // カメラローカル位置
        // ----------------------------
        Vector3 relative = { offsetX * scale,-offsetY * scale,150.0f };
        // ----------------------------
        // カメラ空間→ワールド
        // ----------------------------
        Vector3 targetPos = camera->GetTranslate() + right * relative.x + up * relative.y + forward * relative.z;

        return targetPos;
    }
}