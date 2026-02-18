#pragma once
#include <Vector3.h>

// 前方宣言
class Object3d;
struct Transform;

/// <summary>
/// プレイヤーの移動・傾き制御クラス
/// レールカメラの移動に追従しながら、画面内での相対的な移動と、その入力に応じた機体の傾きを制御する。
/// </summary>
class PlayerMove {
public:// メンバ関数
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    /// <param name="transform">プレイヤーのトランスフォーム</param>
    /// <param name="cameraRotate">現在のカメラの回転角</param>
    /// <param name="speedMultiplier">移動速度の倍率</param>
    void Update(Transform& transform, const Vector3& cameraRotate, float speedMultiplier);
private:
    /// <summary>
    /// プレイヤーが画面外に出ないよう座標を制限する
    /// </summary>
    void ApplyConstraint();
    /// <summary>
    /// 移動入力に基づいた傾き演出の計算
    /// </summary>
    /// <param name="moveInput">現在のフレームの移動量ベクトル</param>
    void UpdateTilt(const Vector3& moveInput);
private:// メンバ変数
    // カメラ座標系における自機の相対座標
    Vector3 relativePos_{};
    // 演出用傾き角
    float tiltX_{}; // 上下移動によるピッチ（機首の上げ下げ）
    float tiltZ_{}; // 左右移動によるロール（翼の傾き）
public: // アクセッサ
    // Getter
    const Vector3& GetLocalPosition() const { return relativePos_; }
	// Setter
    void SetLocalPosition(const Vector3& pos) { relativePos_ = pos; }
};