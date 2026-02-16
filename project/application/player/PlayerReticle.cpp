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