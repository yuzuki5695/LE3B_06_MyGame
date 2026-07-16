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
    public:

        void Update();

        MyEngine::Vector2 WorldToScreen(const MyEngine::Vector3& worldPos, MyEngine::Camera* camera);
    private:
        static constexpr float kMaxOffsetX = 10.0f;
        static constexpr float kMaxOffsetY = 4.0f;
    private:
        MyEngine::Vector2 offset_ = { 0.0f, 0.0f };
        static constexpr float kmovespeed_ = 0.2f;
    public:
        const MyEngine::Vector2& GetOffset() const { return offset_; }

    };
}