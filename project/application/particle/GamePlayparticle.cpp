#include "GamePlayParticle.h"
#include <Player.h>

using namespace MyEngine;

namespace MyGame {

	void GamePlayParticle::Initialize() {
		ParticleManager::GetInstance()->CreateParticleGroup("Explosion", "Models/Particles/Particle.png", "Models/Particles/Particle.obj", 2000);
		ParticleManager::GetInstance()->CreateParticleGroup("Shockwave", "Models/Particles/Shockwave.png", "Models/Particles/Shockwave.obj", 500);
		
		explosionspawndata_.transform = { { 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f },{ 0.5f,0.5f,0.5f }};
		explosionspawndata_.count = 5;
	
		explosionRandom_.count = { 10, 15, true };
		explosionRandom_.scale = { {0.06f,0.06f,0.06f}, {0.1f,0.1f,0.1f}, true };
		explosionRandom_.velocityTranslate = { {-0.1f,-0.1f,-0.1f}, { 0.15f, 0.2f, 0.1f}, true };

		explosionEmitter_ = std::make_unique<BaseEmitter>("Explosion", explosionspawndata_, explosionRandom_);

	}

	void GamePlayParticle::Update() {
		// 通常爆発
		for (const auto& pos : explosionQueue_) {
			explosionEmitter_->SetPosition(pos);
			explosionEmitter_->Emit();
		}

		explosionQueue_.clear();
	}
	
	void GamePlayParticle::AddExplosion(const Vector3& pos) {
		explosionQueue_.push_back(pos);
	}
}