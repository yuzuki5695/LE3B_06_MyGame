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

void PlayerReticle::Initialize(const PlayerReticleData& data) {
    data_ = data;
}

void PlayerReticle::Update(Transform& reticleTransform, const Vector3& playerWorldPos, Object3d* targetObj) {
if (!targetObj) return;
    Input* input = Input::GetInstance();

    // 入力でターゲットを動かす
    // 方向キーでレティクルの相対座標を動かす
    if (input->Pushkey(DIK_LEFT))  reticleTransform.translate.x -= data_.speed;
    if (input->Pushkey(DIK_RIGHT)) reticleTransform.translate.x += data_.speed;
    if (input->Pushkey(DIK_UP))    reticleTransform.translate.y += data_.speed;
    if (input->Pushkey(DIK_DOWN))  reticleTransform.translate.y -= data_.speed;

    // 移動範囲の制限（クランプ)
    reticleTransform.translate.x = std::clamp(reticleTransform.translate.x, data_.minX, data_.maxX);
    reticleTransform.translate.y = std::clamp(reticleTransform.translate.y, data_.minY, data_.maxY);

    // カメラ基準の方向ベクトル計算
    // カメラの回転から、世界軸ではない「現在の視点における正面・右・上」を割り出す
    GamePlayCamera* gameCam = CameraManager::GetInstance()->GetGameplayCamera();
    if (!gameCam) return;
    Vector3 forward = gameCam->GetForward();
    // カメラの向きに基づいた右方向と上方向
    Vector3 right   = Normalize(Cross({0.0f, 1.0f, 0.0f}, forward));
    Vector3 up      = Normalize(Cross(forward, right));

    // プレイヤーの位置から「前方に一定距離」＋「操作による上下左右のオフセット」を加算
    Vector3 offset =
        right * reticleTransform.translate.x +
        up    * reticleTransform.translate.y +
        forward * data_.forwardDistance;

    // 最終的な座標を計算して反映
    Vector3 finalPos = playerWorldPos + offset;
    // 計算した座標を3Dオブジェクトに反映
    targetObj->SetTranslate(finalPos);
    targetObj->Update();
}

void PlayerReticle::UpdateSprite(const Vector3& worldPos, Sprite* targetSprite, Camera* activeCamera) {
    if (!targetSprite || !activeCamera) return; 
    // ビュー・プロジェクション行列を取得
    const Matrix4x4& viewProj = activeCamera->GetViewProjectionMatrix();
    // ワールド座標をクリップ座標系（-1.0 ～ 1.0）へ変換
    Vector4 clipPos = {
        worldPos.x * viewProj.m[0][0] + worldPos.y * viewProj.m[1][0] + worldPos.z * viewProj.m[2][0] + viewProj.m[3][0],
        worldPos.x * viewProj.m[0][1] + worldPos.y * viewProj.m[1][1] + worldPos.z * viewProj.m[2][1] + viewProj.m[3][1],
        worldPos.x * worldPos.y * viewProj.m[0][2] + worldPos.y * viewProj.m[1][2] + worldPos.z * viewProj.m[2][2] + viewProj.m[3][2],
        worldPos.x * viewProj.m[0][3] + worldPos.y * viewProj.m[1][3] + worldPos.z * viewProj.m[2][3] + viewProj.m[3][3]
    };
    // w成分で除算して正規化デバイス座標(NDC)に変換
    if (clipPos.w != 0.0f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
    }
    // スクリーン座標へのマッピング
    // NDC (-1.0 ～ 1.0) を画面解像度 (0 ～ 1280, 0 ～ 720) に変換
    // ※Y軸は上がプラスなので、スプライト（上がマイナス）に合わせて反転させる
    Vector2 screenPos;
    screenPos.x = (clipPos.x * 0.5f + 0.5f) * 1280.0f;
    screenPos.y = (-clipPos.y * 0.5f + 0.5f) * 720.0f;
    // スプライトの位置を更新
    targetSprite->SetPosition(screenPos);
}