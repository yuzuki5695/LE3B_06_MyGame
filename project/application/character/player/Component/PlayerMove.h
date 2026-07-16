#pragma once
#include <BaseCharacter.h>

namespace MyGame {
    /// <summary>
    /// プレイヤーの移動クラス
    /// </summary>
    class PlayerMove {
    public:// メンバ関数
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize();
        /// <summary>
        /// プレイヤーの移動処理 
        /// </summary>
        /// <param name="transform"></param>
        /// <param name="speedMultiplier"></param>
        void Update(MyEngine::Transform& transform, float speedMultiplier);
        /// <summary>
        /// 傾きの更新
        /// </summary>
        /// <param name="moveInput"></param>
        void UpdateTilt(const MyEngine::Vector3& moveInput);
        /// <summary>
        /// ダッシュの状態を更新する
        /// </summary>
        void UpdateDash();
    private: // 内部変数
        /// <summary>
        /// プレイヤーが画面外に出ないよう座標を制限する
        /// </summary>
        void ApplyConstraint();
    private:// メンバ変数
        MyEngine::Vector3 relativePos_{}; // レール上の相対位置
        float tiltX_{}; // 上下移動によるピッチ（機首の上げ下げ）
        float tiltZ_{}; // 左右移動によるロール（翼の傾き）
        bool isDashing_;       // ダッシュ中フラグ
        float dashTimer_;       // ダッシュ経過時間
        float coolDownTimer_;   // ダッシュ再使用までの時間
        // 調整用
        float kDashDuration;    // ダッシュの持続時間
        float kCoolDownTime;    // ダッシュ後のクールダウン時間
        float kDashSpeedMult;   // ダッシュ中の速度倍率
        float extraRotationZ_;  // ダッシュ中の追加回転量
        float speedMultiplier_; // 通常時の速度倍率
    public:
        const MyEngine::Vector3& GetRelativePos() const { return relativePos_; }
        const bool& GetDashing() const { return isDashing_; }
        bool CanDash() const { return coolDownTimer_ <= 0.0f && !isDashing_; }
        float GetCoolDownTimer() const { return coolDownTimer_; }
    };
}