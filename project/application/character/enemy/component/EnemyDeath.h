#pragma once
#include <Object3d.h>
#include <random>

namespace MyGame {

    class EnemyDeath {
    public:

        void Start();
        void Update(MyEngine::Object3d* object);

    private:

        bool isPlaying_;
        bool isfinished_;

        float timer_;
        float duration_;

        std::mt19937 randomEngine_{ std::random_device{}() };
    public:
        bool IsFinished() const { return isfinished_; }
    };
}