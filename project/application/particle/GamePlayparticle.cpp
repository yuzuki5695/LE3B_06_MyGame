#include "GamePlayParticle.h"

using namespace MyEngine;

namespace MyGame {

	void GamePlayParticle::Initialize() {
		ParticleManager::GetInstance()->CreateParticleGroup("Explosion", "Models/Particles/Particle.png", "Models/Particles/Particle.obj", 300);
		ParticleManager::GetInstance()->CreateParticleGroup("Shockwave", "Models/Particles/Shockwave.png", "Models/Particles/Shockwave.obj", 300);
		
		explosionspawndata_.transform = { { 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f },{ 0.1f,0.1f,0.1f }};
		explosionspawndata_.count = 5;
	
		explosionRandom_.count = { 10, 15, true };
		explosionRandom_.scale = { {0.06f,0.06f,0.06f}, {0.1f,0.1f,0.1f}, true };
		explosionRandom_.velocityTranslate = { {-0.1f,-0.1f,-0.1f}, { 0.15f, 0.2f, 0.1f}, true };

		explosionEmitter_ = std::make_unique<BaseEmitter>("Explosion", explosionspawndata_, explosionRandom_);
		//	
		//shockwaveRandom_.count = { 2, 5, true };
		//shockwaveRandom_.scale = { {0.2f,0.2f,0.2f}, {0.3f,0.3f,0.3f}, true };
		//shockwaveRandom_.rotate = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,360.0f}, true };
		//shockwaveRandom_.velocityScale = { {0.0f,0.0f,0.0f} ,{0.006f,0.006f,0.006f},true };

		//shockwaveEmitter_ = std::make_unique<BaseEmitter>("Shockwave", spawnData, shockwaveRandom_);
	}

	void GamePlayParticle::Update() {

		for (const auto& pos : explosionQueue_) {
			explosionEmitter_->SetPosition(pos);
			explosionEmitter_->Emit();
			//shockwaveEmitter_->SetPosition(pos);
			//shockwaveEmitter_->Emit();
		}

		explosionQueue_.clear();
	}
}