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

        void UpdateTilt(const MyEngine::Vector3& moveInput);

    private: // 内部変数
        /// <summary>
        /// プレイヤーが画面外に出ないよう座標を制限する
        /// </summary>
        void ApplyConstraint();
    private:// メンバ変数
        MyEngine::Vector3 relativePos_{}; // レール上の相対位置
        float tiltX_{}; // 上下移動によるピッチ（機首の上げ下げ）
        float tiltZ_{}; // 左右移動によるロール（翼の傾き）
    public:
		const MyEngine::Vector3& GetRelativePos() const { return relativePos_; }
    };
}