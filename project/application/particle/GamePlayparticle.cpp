#include "GamePlayParticle.h"
#include <Player.h>

using namespace MyEngine;

namespace MyGame {

	void GamePlayParticle::Initialize() {
		// パーティクルグループの作成
		ParticleManager::GetInstance()->CreateParticleGroup("Explosion", "Models/Particles/Particle.png", "Models/Particles/Particle.obj", 2000);
		ParticleManager::GetInstance()->CreateParticleGroup("Shockwave", "Models/Particles/Shockwave.png", "Models/Particles/Shockwave.obj", 500);

		//初期パラメータ
		explosionspawndata_.transform = { { 0.2f,0.2f,0.2f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } };
		explosionspawndata_.count = 30;
		explosionspawndata_.behavior = ParticleBehavior::Attract;
		explosionspawndata_.lifetime = 4.0f;
		// ランダムパラメータ
		explosionRandom_.count = { 10, 10, true };
		explosionRandom_.scale = { {0.06f,0.06f,0.06f}, {0.1f,0.1f,0.1f}, true };
		explosionRandom_.velocityTranslate = { {-0.1f,-0.1f,-0.1f}, { 0.15f, 0.2f, 0.1f}, true };
		explosionRandom_.stopTime = { 1.0f, 2.0f, true };
		// パーティクルの生成
		explosionEmitter_ = std::make_unique<BaseEmitter>("Explosion", explosionspawndata_, explosionRandom_);
		// 1回だけ呼ぶ(ウォームアップ)
		explosionEmitter_->SetPosition({99999,99999,99999});
		explosionEmitter_->Emit();

		////初期パラメータ
		//shockwavedata_.transform = { { 0.5f,0.5f,0.5f },    { 0.0f,0.0f,0.0f },    { 0.0f,0.0f,0.0f } };
		//shockwavedata_.count = 5;
		//shockwavedata_.behavior = ParticleBehavior::Normal;
		//shockwavedata_.lifetime = 0.4f;
		//// ランダムパラメータ
		//shockwaveRandom_.rotate = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,360.0f}, true };
		//shockwaveRandom_.scale = { {0.02f,0.02f,0.02f}, {0.10f,0.10f,0.10f}, true };
		//shockwaveRandom_.lifetime = { 0.3f, 0.5f, true };
		//shockwaveRandom_.velocityScale = { {0.006f,0.006f,0.006f}, {0.06f,0.06f,0.06f}, true };
		//shockwaveRandom_.count = { 3, 5, true };

		//// パーティクルの生成
		//shockwaveEmitter_ = std::make_unique<BaseEmitter>("Shockwave", shockwavedata_, shockwaveRandom_);
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