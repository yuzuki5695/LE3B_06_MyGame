#pragma once
#include <BaseCharacter.h>

namespace MyGame {
    /// <summary>
    /// プレイヤーの移動クラス
    /// </summary>
    class PlayerMove {
    public:// メンバ関数
        /// <summary>
        /// プレイヤーの移動処理 
        /// </summary>
        /// <param name="transform"></param>
        /// <param name="speedMultiplier"></param>
        void Update(MyEngine::Transform& transform, float speedMultiplier);
		// 傾きの更新
        void UpdateTilt(const MyEngine::Vector3& moveInput);
		// ダッシュの状態を更新する
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

        bool isDashing_ = false;

        float dashTimer_ = 0.0f;
        float coolDownTimer_ = 0.0f;

        // 調整用
		const float kDashDuration = 0.5f; // ダッシュの持続時間
		const float kCoolDownTime = 3.0f; // ダッシュ後のクールダウン時間
		const float kDashSpeedMult = 2.0f; // ダッシュ中の速度倍率

		float extraRotationZ_ = 0.0f; // ダッシュ中の追加回転量
		float speedMultiplier_ = 1.0f; // 通常時の速度倍率

    public:
        const MyEngine::Vector3& GetRelativePos() const { return relativePos_; }
    };
}