#pragma once
#include <Vector3.h>

// 前方宣言
struct Transform;
class Camera;
class Sprite;
class Object3d;

/// <summary>
/// プレイヤーのレティクル（照準）計算クラス
/// </summary>
class PlayerReticle {
public:
    /// <summary>
    /// レティクルの座標更新
    /// </summary>
    /// <param name="reticleTransform">更新対象のトランスフォーム</param>
    /// <param name="playerLocalPos">PlayerMoveから取得した自機のローカル座標</param>
    /// <param name="activeCamera">現在使用中のカメラ</param>
    void Update(Transform& reticleTransform, const Vector3& playerWorldPos, Object3d* targetObj);
private:
    // レティクルの移動制限範囲
const float kSpeed = 0.4f;
    const float kMaxX = 12.0f;
    const float kMinX = -12.0f;
    const float kMaxY = 7.0f;
    const float kMinY = -7.0f;
    const float kForwardDistance = 30.0f;
};