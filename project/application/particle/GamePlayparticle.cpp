#include "GamePlayParticle.h"
#include <Player.h>
#include <MatrixVector.h>
#include <CameraManager.h>
#include <SceneEmitterManager.h>

using namespace MyEngine::MatrixVector;

using namespace MyEngine;

namespace MyGame {

	void GamePlayParticle::Initialize() {
		// パーティクルグループの作成
		ParticleManager::GetInstance()->CreateParticleGroup("Explosion", "Models/Particles/Particle.png", "Models/Particles/Particle.obj", 2000);
		ParticleManager::GetInstance()->CreateParticleGroup("Shockwave", "Models/Particles/Shockwave.png", "Models/Particles/Shockwave.obj", 500);
		ParticleManager::GetInstance()->CreateParticleGroup("Player", "Models/Particles/Particle_02.png", "Models/Particles/Particle_02.obj", 2000);

		//初期パラメータ
		explosionspawndata_.transform = { { 0.1f,0.1f,0.1f },{ 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f } };
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
		explosionEmitter_->SetTranslate({ 99999,99999,99999 });
		explosionEmitter_->Emit();

		//初期パラメータ
		playerspawndata_.transform = { { 0.04f,0.04f,0.04f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
		playerspawndata_.count = 1;
		playerspawndata_.behavior = ParticleBehavior::Normal;
		playerspawndata_.lifetime = 0.1f;
		// ランダムパラメータ
		playerrandom_.translate = { {-0.1f,-0.1f,-0.1f}, {0.1f,0.1f,0.1f}, true };
		playerrandom_.velocityTranslate = { {-0.05f,-0.05f,0.05f}, {0.05f,0.05f,0.1f}, true };
		//playerrandom_.scale ={ {-0.02f,-0.06f,-0.02f}, {0.02f,-0.02f,0.02f}, true };
		playerrandom_.lifetime = { 0.1f, 0.5f, true };
		playerrandom_.count = { 3, 6, true };
		// タイマーの設定
		playerEmitTimer_ = 0.0f;
		playerEmitInterval_ = 0.01f;

		// パーティクルの生成
		playeremitter_ = std::make_unique<BaseEmitter>("Player", playerspawndata_, playerrandom_);
	}

	void GamePlayParticle::Update() {
		// 通常爆発
		for (const auto& pos : explosionQueue_) {
			explosionEmitter_->SetTranslate(pos);
			explosionEmitter_->Emit();
		}

		explosionQueue_.clear();
		// プレイヤートレイル
		playerEmitTimer_ += 1.0f / 60.0f; // or deltaTime

		if (playerEmitTimer_ >= playerEmitInterval_) {
			EmitPlayerTrail();
			playerEmitTimer_ = 0.0f;
		}
	}

	void GamePlayParticle::AddExplosion(const Vector3& pos) {
		explosionQueue_.push_back(pos);
	}

	void GamePlayParticle::EmitPlayerTrail() {
		Vector3 pos =  SceneEmitterManager::GetInstance()->GetWorldPosition();
		Vector3 dir = SceneEmitterManager::GetInstance()->GetForward();

		Vector3 emitPos = pos - dir * 1.5f;
		emitPos.y -= 0.5f;
		emitPos.z += 0.5f;

		playeremitter_->SetTranslate(emitPos);
		playeremitter_->SetDirection(-dir);
		playeremitter_->Emit();

	}
}