#include "PlayerReticle.h"
#include <Transform.h>
#include <Camera.h>
#include <MatrixVector.h>
#include <algorithm>
#include <Sprite.h>
#include <Object3d.h>
#include <Input.h>
#include <CameraManager.h>

using namespace MatrixVector;

void PlayerReticle::Update(Transform& reticleTransform, const Vector3& playerWorldPos, Object3d* targetObj) {
if (!targetObj) return;
    Input* input = Input::GetInstance();

    // --- 1. 入力でターゲットを動かす ---
    if (input->Pushkey(DIK_LEFT))  reticleTransform.translate.x -= kSpeed;
    if (input->Pushkey(DIK_RIGHT)) reticleTransform.translate.x += kSpeed;
    if (input->Pushkey(DIK_UP))    reticleTransform.translate.y += kSpeed; // 上
    if (input->Pushkey(DIK_DOWN))  reticleTransform.translate.y -= kSpeed; // 下

    // --- 2. 範囲内にクランプ ---
    reticleTransform.translate.x = std::clamp(reticleTransform.translate.x, kMinX, kMaxX);
    reticleTransform.translate.y = std::clamp(reticleTransform.translate.y, kMinY, kMaxY);

    // --- 3. カメラ基準の方向ベクトル計算 ---
    GamePlayCamera* gameCam = CameraManager::GetInstance()->GetGameplayCamera();
    if (!gameCam) return;

    Vector3 forward = gameCam->GetForward();
    // カメラの向きに基づいた右方向と上方向
    Vector3 right   = Normalize(Cross({0.0f, 1.0f, 0.0f}, forward));
    Vector3 up      = Normalize(Cross(forward, right));

    // --- 4. プレイヤー位置を基準とした配置計算 ---
    Vector3 offset =
        right * reticleTransform.translate.x +
        up    * reticleTransform.translate.y +
        forward * kForwardDistance;

    // 最終的な座標を計算して反映
    Vector3 finalPos = playerWorldPos + offset;
    
    targetObj->SetTranslate(finalPos);
    targetObj->Update();
}

void PlayerReticle::UpdateSprite(const Vector3& worldPos, Sprite* targetSprite, Camera* activeCamera) {
    if (!targetSprite || !activeCamera) return;

    // --- D. 3D→2D変換 (Player.cpp の UpdateReticlePosition を移植) ---
    const Matrix4x4& viewProj = activeCamera->GetViewProjectionMatrix();

    Vector4 clipPos = {
        worldPos.x * viewProj.m[0][0] + worldPos.y * viewProj.m[1][0] + worldPos.z * viewProj.m[2][0] + viewProj.m[3][0],
        worldPos.x * viewProj.m[0][1] + worldPos.y * viewProj.m[1][1] + worldPos.z * viewProj.m[2][1] + viewProj.m[3][1],
        worldPos.x * worldPos.y * viewProj.m[0][2] + worldPos.y * viewProj.m[1][2] + worldPos.z * viewProj.m[2][2] + viewProj.m[3][2],
        worldPos.x * viewProj.m[0][3] + worldPos.y * viewProj.m[1][3] + worldPos.z * viewProj.m[2][3] + viewProj.m[3][3]
    };

    if (clipPos.w != 0.0f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
    }

    Vector2 screenPos;
    screenPos.x = (clipPos.x * 0.5f + 0.5f) * 1280.0f;
    screenPos.y = (-clipPos.y * 0.5f + 0.5f) * 720.0f;

    targetSprite->SetPosition(screenPos);
}