#pragma once
#include <Vector3.h>

// 前方宣言
struct Transform;
class Camera;
class Sprite;

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
    void Update(Transform& reticleTransform, const Vector3& playerLocalPos, Camera* activeCamera);
    

    // 1. 3Dレティクルのワールド座標を計算して反映する
    void Update3DPosition(Transform& reticleTransform, const Vector3& playerLocalPos, Camera* camera);

    // 2. 3D座標を2Dスクリーン座標に変換してSpriteに反映する
    void Update2DSprite(const Vector3& worldPos, Sprite* uiReticle, Camera* camera);
private:
    // レティクルの移動制限範囲
    const float kLimitX = 14.0f;
    const float kLimitY_Min = -4.0f;
    const float kLimitY_Max = 10.0f;


    const float kDistanceZ = 50.0f;
};