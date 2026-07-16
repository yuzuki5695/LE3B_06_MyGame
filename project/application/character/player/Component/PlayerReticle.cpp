#include "PlayerReticle.h"
#include <Input.h>
#include <MatrixVector.h>
#include <algorithm>
#include <MathUtil.h>

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