#pragma once
#include <Vector3.h>

/// <summary>
/// プレイヤーのダッシュ（回避・高速移動）管理クラス
/// 一定時間速度を上げ、機体をロール（回転）させる演出を制御する。
/// </summary>
class PlayerDash {
public:
    /// <summary>
    /// 初期化（パラメータのリセット）
    /// </summary>
    void Initialize();
    /// <summary>
    /// ダッシュの開始
    /// </summary>
    /// <param name="directionX">現在の左右入力値（回転方向を決定するために使用）</param>
    void Start(float directionX);
    /// <summary>
    /// ダッシュ状態とクールタイムの更新
    /// </summary>
    void Update();
private:
    // --- 状態管理 ---
    bool isDashing_ = false;       // 現在ダッシュ中か
    float dashTimer_ = 0.0f;       // ダッシュ開始からの経過時間
    float coolDownTimer_ = 0.0f;   // 再使用までの残り時間
    // --- 設定定数 ---
    const float kDashDuration = 0.5f;   // ダッシュ（回転）が続く時間
    const float kCoolDownTime = 3.0f;   // ダッシュ終了後のクールタイム
    const float kMaxSpeedMult = 2.5f;   // ダッシュ中の最大速度倍率
    // --- 演出用変数 ---
    float dashDirX_ = 0.0f;        // 回転する方向（左:-1.0, 右:1.0）
    float extraRotationZ_ = 0.0f;  // ダッシュによる追加のZ軸回転量（ラジアン）
    float speedMultiplier_ = 1.0f; // 移動クラスに渡す速度倍率


public: // アクセッサ
	// Getter
    // 現在の回転角を取得（PlayerMoveの回転に加算する）
    float GetRotationZ() const { return extraRotationZ_; } 
    // 現在の速度倍率を取得（PlayerMoveの移動量に掛ける）
    float GetSpeedMultiplier() const { return speedMultiplier_; }    
    // クールタイムの進捗（0.0～1.0） ※UIのゲージ表示などに利用
    float GetCoolDownProgress() const { return coolDownTimer_ / kCoolDownTime; }
    
    // ダッシュ中かどうか
    bool IsDashing() const { return isDashing_; }
    // 現在ダッシュ可能か（ダッシュ中でなく、かつクールタイムが終了しているか）
    bool CanDash() const { return !isDashing_ && coolDownTimer_ <= 0.0f; }    
};