#include "GamePlayparticle.h"
using namespace MyEngine;


namespace MyGame {

    void GamePlayparticle::Initialize(Object3d* target) {
        // パーティクルグループ生成
        ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Particle.png", "Particle.obj");
        // =========================
        // エミッター生成
        // =========================
        Transform emitterTransform{};
        emitterTransform.translate = target->GetTranslate();
        emitterTransform.scale = { 0.0f, 0.0f, 0.0f };

        Velocity velocity{};
        velocity.translate = { 0.01f, 0.05f, 0.0f }; // 上方向に飛ぶ
        velocity.rotate = { 0.0f, 0.0f, 0.0f };
        velocity.scale = { 0.0f, 0.0f, 0.0f };

        ParticleSpawnData spawnData;
        spawnData.transform = emitterTransform;
        spawnData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        spawnData.count = 5;
        spawnData.velocity = velocity;
        spawnData.lifetime = 1.5f;
        spawnData.useGravity = false;

        // 位置のばらつき
        random_.count = { 1, 5, true };
        random_.translate = { { -3.0f, -3.0f, -3.0f },{ 3.0f, 3.0f, 3.0f },true };
        random_.scale = { { 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },true };
        random_.lifetime = { 0.3f, 3.0f, true };
        particleEmitter_ = std::make_unique<ParticleEmitter>("Particles", spawnData, random_, 1.0f);
    }

    void GamePlayparticle::Update() {

        particleEmitter_->Update();
    }
}