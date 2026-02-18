#pragma once
#include <Vector3.h>

// 前方宣言
struct Transform;
class Camera;
class Sprite;
class Object3d;

/// <summary>
/// プレイヤーのレティクル（照準）計算クラス
/// 3D空間上のターゲット位置の計算と、それを2D画面上のスプライトへ変換する機能を持つ。
/// </summary>
class PlayerReticle {
public:
    /// <summary>
    /// レティクルの3D座標更新
    /// キー入力に応じて照準を移動させ、プレイヤーの前方に配置する。
    /// </summary>
    /// <param name="reticleTransform">レティクルの相対トランスフォーム（入力で書き換わる）</param>
    /// <param name="playerWorldPos">プレイヤーの現在のワールド座標</param>
    /// <param name="targetObj">描画用の3Dレティクルオブジェクト</param>
    void Update(Transform& reticleTransform, const Vector3& playerWorldPos, Object3d* targetObj);

    /// <summary>
    /// 3D座標から2D座標への変換処理
    /// 3D空間上のレティクル位置をスクリーン座標に変換し、2Dスプライトの位置を同期させる。
    /// </summary>
    /// <param name="worldPos">変換元の3Dワールド座標</param>
    /// <param name="targetSprite">表示する2Dスプライト</param>
    /// <param name="activeCamera">レンダリングに使用しているカメラ</param>
    void UpdateSprite(const Vector3& worldPos, Sprite* targetSprite, Camera* activeCamera);

private:
    // --- 設定定数 ---
    const float kSpeed = 0.4f;            // レティクルの移動速度
    const float kMaxX = 12.0f;           // 左右の移動限界
    const float kMinX = -12.0f;
    const float kMaxY = 7.0f;            // 上下の移動限界
    const float kMinY = -7.0f;
    const float kForwardDistance = 30.0f; // プレイヤーからどれくらい前方に配置するか
};