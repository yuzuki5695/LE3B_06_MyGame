#include "PlayerReticle.h"
#include <Transform.h>
#include <Camera.h>
#include <MatrixVector.h>
#include <algorithm>
#include <Sprite.h>

using namespace MatrixVector;

void PlayerReticle::Update(Transform& reticleTransform, const Vector3& playerLocalPos, Camera* activeCamera) {
    if (!activeCamera) return;

    // 1. 自機の座標に追従（自機より少し前方に配置するなどのオフセット計算）
    // Player.cpp にあった 3Dレティクルの計算ロジックをここに集約
    Vector3 offset = { 0.0f, 0.0f, 50.0f }; // 例：50m前方に配置
    
    // 自機のローカル位置に基づいた制限付き移動
    Vector3 targetPos = playerLocalPos;
    targetPos.x = std::clamp(targetPos.x * 1.5f, -kLimitX, kLimitX);
    targetPos.y = std::clamp(targetPos.y * 1.2f, kLimitY_Min, kLimitY_Max);
    targetPos.z += offset.z;

    // 2. カメラの行列を使ってワールド座標へ変換
    Matrix4x4 matCamera = MakeAffineMatrix(
        {1,1,1}, 
        activeCamera->GetRotate(), 
        activeCamera->GetTranslate()
    );
    
    // 最終的なワールド座標を適用
    reticleTransform.translate = Multiply4x4x3(matCamera, targetPos);
}

// --- 3D位置の更新 ---
void PlayerReticle::Update3DPosition(Transform& reticleTransform, const Vector3& playerLocalPos, Camera* camera) {
    if (!camera) return;

    // 以前のロジック：自機の相対座標に倍率をかけて制限
    Vector3 localTarget;
    localTarget.x = std::clamp(playerLocalPos.x * 1.5f, -kLimitX, kLimitX);
    localTarget.y = std::clamp(playerLocalPos.y * 1.2f, kLimitY_Min, kLimitY_Max);
    localTarget.z = playerLocalPos.z + kDistanceZ;

    // カメラのワールド行列を取得して変換
    Matrix4x4 matCamera = MakeAffineMatrix({1,1,1}, camera->GetRotate(), camera->GetTranslate());
    reticleTransform.translate = Multiply4x4x3(matCamera, localTarget);
}

// --- 2Dスプライトへの反映 ---
void PlayerReticle::Update2DSprite(const Vector3& worldPos, Sprite* uiReticle, Camera* camera) {
    if (!uiReticle || !camera) return;

    const Matrix4x4& viewProj = camera->GetViewProjectionMatrix();

    // 以前の変換式を完全再現
    Vector4 clipPos = {
        worldPos.x * viewProj.m[0][0] + worldPos.y * viewProj.m[1][0] + worldPos.z * viewProj.m[2][0] + viewProj.m[3][0],
        worldPos.x * viewProj.m[0][1] + worldPos.y * viewProj.m[1][1] + worldPos.z * viewProj.m[2][1] + viewProj.m[3][1],
        worldPos.x * viewProj.m[0][2] + worldPos.y * viewProj.m[1][2] + worldPos.z * viewProj.m[2][2] + viewProj.m[3][2],
        worldPos.x * viewProj.m[0][3] + worldPos.y * viewProj.m[1][3] + worldPos.z * viewProj.m[2][3] + viewProj.m[3][3]
    };

    if (clipPos.w != 0.0f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
    }

    // 以前のスクリーン座標計算
    Vector2 screenPos;
    screenPos.x = (clipPos.x * 0.5f + 0.5f) * 1280.0f;
    screenPos.y = (-clipPos.y * 0.5f + 0.5f) * 720.0f;

    uiReticle->SetPosition(screenPos);
}
