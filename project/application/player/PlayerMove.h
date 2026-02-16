#pragma once
#include <Vector3.h>

// 前方宣言
class Object3d;
struct Transform;

/// <summary>
/// プレイヤーの移動・傾き制御クラス
/// 入力に基づいた座標計算、移動範囲の制限、および移動に伴う機体の傾き演出を行う。
/// </summary>
class PlayerMove {
public:// メンバ関数
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    /// <param name="transform">プレイヤーのトランスフォーム（参照）</param>
    /// <param name="cameraRotate">現在のカメラ回転角</param>
    void Update(Transform& transform, const Vector3& cameraRotate);
private:
    /// <summary>
    /// 移動範囲の制限を適用する
    /// </summary>
    void ApplyConstraint();
    /// <summary>
    /// 移動入力に基づいた傾き演出の計算
    /// </summary>
    /// <param name="moveInput">現在の移動入力ベクトル</param>
    void UpdateTilt(const Vector3& moveInput);
private:// メンバ変数
    // カメラ座標系における自機の相対座標
    Vector3 relativePos_{};
    // 現在の傾き角（X軸：ピッチ、Z軸：ロール）
    float tiltX_{};
    float tiltZ_{};
public:// メンバ関数
    const Vector3& GetLocalPosition() const { return relativePos_; }
	void SetLocalPosition(const Vector3& pos) { relativePos_ = pos; }
};