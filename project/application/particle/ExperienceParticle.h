#pragma once
#include <Vector3.h>

namespace MyGame {

    class Player;

    class ExperienceParticle {
    public:
        enum class State {
            Scatter,
            Wait,
            Follow
        };
        enum class AttractMode {
            None,
            ToPosition
        };
        void Initialize(const MyEngine::Vector3& startPos, const MyEngine::Vector3& targetPos);
        void Update();

        bool IsDead() const { return isDead_; }

        MyEngine::Vector3 GetPosition() const { return position_; }

    private:
        State state_ = State::Scatter;
        AttractMode attractMode_ = AttractMode::None;
        MyEngine::Vector3 attractPos_;

        MyEngine::Vector3 position_;
        MyEngine::Vector3 velocity_;
        MyEngine::Vector3 waitPosition_;
        
        MyEngine::Vector3 targetPos_;
        float timer_ = 0.0f;
        bool isDead_ = false;
    };
}