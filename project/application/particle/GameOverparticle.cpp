#include "GameOverparticle.h"
#include <numbers>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

void GameOverparticle::Initialize(const std::vector<Object3d*>& targets) {
    // パーティクルグループ生成
    ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Resources/uvChecker.png", "Particle.obj", VertexType::Model);

    random_
        .SetOffset({ -0.3f,-0.01f,-0.3f }, { 0.3f,0.3f,0.3f })
        .SetColor(0.0f, 0.5f)
        .SetVelocity({ -0.02f,-0.01f,-0.02f }, { 0.02f,0.01f,0.02f })
        .SetLifetime(-0.3f,0.1f);

    for (auto* target : targets) {
        auto emitter = std::make_unique<FollowEmitter>(
            "Particles",
            1,
            Transform{{0.1f, 0.1f, 0.1f}, {0,0,0}, {0,0,0}},
            Vector4{1,1,1,1},
            1.0f,
            0.0f,
            Velocity{{0.0f, 0.06f, 0.0f},{0,0,0},{0,0,0}},
            random_
        );

        emitter->SetTarget(target);
        emitter->maxParticles_ = 400;
        emitter->emitPerTick_ = 2;
        emitter->SetOffset({0.0f, 0.0f, 0.0f});
        emitters_.push_back(std::move(emitter));
    }
}

void GameOverparticle::Update() {
    for (auto& e : emitters_) {
       e->Update();
    }

#ifdef USE_IMGUI
    for (auto& e : emitters_) {
 //       e->DrawImGuiUI();
    }
#endif
}