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
        float ndcX = (screenPos.x / 1280.0f) * 2.0f - 1.0f;
        float ndcY = -(screenPos.y / 720.0f) * 2.0f + 1.0f;

        Matrix4x4 invViewProj = Inverse(camera->GetViewProjectionMatrix());

        Vector4 nearPos = { ndcX, ndcY, 0.0f, 1.0f };
        Vector4 farPos = { ndcX, ndcY, 1.0f, 1.0f };

        // ワールド変換
        nearPos = TransformCoord(nearPos, invViewProj);
        farPos = TransformCoord(farPos, invViewProj);

        nearPos = nearPos / nearPos.w;
        farPos = farPos / farPos.w;

        Vector3 rayDir = Normalize(Vector3{
            farPos.x - nearPos.x,
            farPos.y - nearPos.y,
            farPos.z - nearPos.z
            });
        
        Vector3 camPos = camera->GetTranslate();

        // 適当な距離
        return camPos + rayDir * kForwardDistance;
    }
}