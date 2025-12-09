#include "GameOverparticle.h"
#include <numbers>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

void GameOverparticle::Initialize(const std::vector<Object3d*>& targets) {
    // パーティクルグループ生成
    ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Resources/Particle.png", "Particle.obj", VertexType::Model);
    ParticleManager::GetInstance()->CreateParticleGroup("Particles_02", "Resources/Particle_02.png", "Particle_02.obj", VertexType::Model);
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
        emitter->maxParticles_ = 500;
        emitter->emitPerTick_ = 3;
        emitter->SetOffset({0.0f, 0.0f, 0.0f});
        emitters_.push_back(std::move(emitter));
    }

    for (auto* target : targets) {
        auto emitter = std::make_unique<FollowEmitter>(
            "Particles_02",
            1,
            Transform{ {0.1f, 0.1f, 0.1f}, {0,0,0}, {0,0,0} },
            Vector4{ 1,1,1,1 },
            0.7f,
            0.0f,
            Velocity{ {0.0f, 0.06f, 0.0f},{0,0,0},{0,0,0} },
            random_
        );

        emitter->SetTarget(target);
        emitter->maxParticles_ = 250;
        emitter->emitPerTick_ = 1;
        emitter->SetOffset({ 0.0f, 0.0f, 0.0f });
        emitters2_.push_back(std::move(emitter));
    }
}

void GameOverparticle::Update() {
    for (auto& e : emitters_) {
       e->Update();
    }
    for (auto& e : emitters2_) {
       e->Update();
    }
#ifdef USE_IMGUI
    for (auto& e : emitters_) {
 //       e->DrawImGuiUI();
    }
#endif
}

void GameOverparticle::Finalize() {
    emitters_.clear();
    emitters2_.clear();
}