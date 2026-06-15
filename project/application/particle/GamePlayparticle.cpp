#include "GamePlayparticle.h"
using namespace MyEngine;


namespace MyGame {

    void GamePlayparticle::Initialize(Object3d* target) {
        // パーティクルグループ生成
        //ParticleManager::GetInstance()->CreateParticleGroup("Firework", "Models/Particles/Particle.png", "Models/Particles/Particle.obj", 300);
        //ParticleManager::GetInstance()->CreateParticleGroup("Shockwave", "Models/Particles/Shockwave.png", "Models/Particles/Shockwave.obj", 300);     

        ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Models/Particles/Particle.png", "Models/Particles/Particle.obj", 300);

        // エミッター生成     
        Transform emitterTransform{};
        emitterTransform.translate = target->GetTranslate();
        emitterTransform.translate.z += 30.0f;
        emitterTransform.scale = { 1.0f, 1.0f, 1.0f };

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
        random_.translate = { { -3.0f, 0.0f, 0.0f },{ 3.0f, 0.0f, 0.0f },true };
        particleEmitter_ = std::make_unique<ParticleEmitter>("Particles", spawnData, random_, 1.0f);
    }

    void GamePlayparticle::Update() {

        particleEmitter_->Update();
    }
}