#include "DefaultParticleEmitter.h"
#include <numbers>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

void DefaultParticleEmitter::Update() {
    // 時間加算
    timer_ += 1.0f / 60.0f;

    // 一定時間経過したら爆発
    if (timer_ >= explodeDelay_ && !exploded_) {
        // 発生パーティクル数分 Emit
        for (uint32_t i = 0; i < count_; ++i) {
            Emit();
        }
        // タイマーリセット
        timer_ = 0.0f;
        exploded_ = true;
    } 
}

bool DefaultParticleEmitter::IsDead() const {
    return frequency_ <= 0.0f;
}