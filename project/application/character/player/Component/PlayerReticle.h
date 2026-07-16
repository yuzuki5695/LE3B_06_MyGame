#pragma once
#include <Vector2.h>
#include <Vector3.h>
#include <Transform.h>
#include <Camera.h>

namespace MyGame {
    /// <summary>
    /// プレイヤーのレティクルクラス
    /// </summary>
    class PlayerReticle {
    public: // メンバ関数
        /// <summary>
        /// レティクル位置を更新する
        /// </summary>
        void Update();
        /// <summary>
        /// ワールド座標をスクリーン座標へ変換する
        /// </summary>
        /// <param name="worldPos">変換するワールド座標</param>
        /// <param name="camera">使用するカメラ</param>
        /// <returns>スクリーン座標</returns>
        MyEngine::Vector2 WorldToScreen(const MyEngine::Vector3& worldPos, MyEngine::Camera* camera);
    private: // メンバ変数
        static constexpr float kMaxOffsetX = 10.0f;       // レティクル移動可能範囲(X)
        static constexpr float kMaxOffsetY = 4.0f;        // レティクル移動可能範囲(Y)
        static constexpr float kmovespeed_ = 0.2f;        // レティクル移動速度
    private:
        MyEngine::Vector2 offset_ = { 0.0f, 0.0f };  // レティクルのオフセット位置
    public:
        const MyEngine::Vector2& GetOffset() const { return offset_; }

    };
}