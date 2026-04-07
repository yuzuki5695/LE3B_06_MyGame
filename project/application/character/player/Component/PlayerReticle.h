#pragma once
#include <Vector2.h>
#include <Vector3.h>
#include <Transform.h>
#include <Camera.h>

namespace MyGame {
    /// <summary>
    /// プレイヤーのレティクル（照準）計算クラス
    /// 3D空間上のターゲット位置の計算と、それを2D画面上のスプライトへ変換する機能を持つ。
    /// </summary>
    class PlayerReticle {
    public:

        void UpdateInput();

        MyEngine::Vector3 ScreenToWorld(const MyEngine::Vector2& screenPos, MyEngine::Camera* camera);

    private:
        MyEngine::Vector2 screenPos_ = { 640.0f, 360.0f }; // 画面中央
        float moveSpeed_ = 10.0f;
        const float kForwardDistance = 30.0f; // プレイヤーからどれくらい前方に配置するか
    public:
        const MyEngine::Vector2& GetScreenPos() const { return screenPos_; }

    };
}