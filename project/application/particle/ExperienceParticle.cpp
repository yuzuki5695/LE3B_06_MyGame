#include "ExperienceParticle.h"
#include <MatrixVector.h>
#include <ParticleManager.h>

using namespace MyEngine;
using namespace MatrixVector;

namespace MyGame {

    void ExperienceParticle::Initialize(const Vector3& startPos, const Vector3& targetPos) {
        position_ = startPos;
        targetPos_ = targetPos;
        auto& pm = *ParticleManager::GetInstance();

        velocity_ = { pm.Rand(-0.15f,0.15f), pm.Rand(0.05f,0.2f), pm.Rand(-0.15f,0.15f) };
    }

    void ExperienceParticle::Update() {
        timer_ += 1.0f / 60.0f;

        switch (state_) {
        case State::Scatter: {
            position_ += velocity_;

            // 減速
            velocity_ *= 0.92f;

            if (timer_ >= 0.2f) {
                timer_ = 0.0f;
                state_ = State::Wait;
                waitPosition_ = position_;
            }
            break;
        } case State::Wait: {
            if (timer_ >= 0.15f) {
                timer_ = 0.0f;
                state_ = State::Follow;
            }
            break;
        } case State::Follow: {

            Vector3 toTarget = attractPos_ - position_;
            float dist = Length(toTarget);
            Vector3 dir = Normalize(toTarget);
            float speed = std::clamp(dist * 0.08f, 0.12f, 1.5f);
            position_ += dir * speed;
            if (dist < 0.4f) {
                isDead_ = true;
            }
            break;
        }
        }
    }
}