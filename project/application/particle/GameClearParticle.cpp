#include "GameClearParticle.h"
#include <MatrixVector.h>
#include <CameraManager.h>
#include <SceneEmitterManager.h>

using namespace MyEngine::MatrixVector;

using namespace MyEngine;

namespace MyGame {

	void GameClearParticle::Initialize() {
		// パーティクルグループの作成
		ParticleManager::GetInstance()->CreateParticleGroup("Clear", "Models/Particles/Particle_02.png", "Models/Particles/Particle_02.obj", 1000);

		//初期パラメータ
		playerspawndata_.transform = { { 0.04f,0.04f,0.04f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
		playerspawndata_.count = 2;
		playerspawndata_.behavior = ParticleBehavior::Normal;
		playerspawndata_.lifetime = 1.0f;
		// ランダムパラメータ
		playerrandom_.translate = { {-0.1f,-0.1f,-0.1f}, {0.1f,0.1f,0.1f}, true };
		playerrandom_.velocityTranslate = { {-0.02f,-0.05f,-0.7f}, {0.02f,0.05f,0.0f}, true };
		playerrandom_.lifetime = { 0.5f, 1.5f, true };
		playerrandom_.count = { 8, 16, true };
		// タイマーの設定
		emitTimer_ = 0.0f;
		emitInterval_ = 0.05f;
		deltaTime = 0.0f;
		// パーティクルの生成
		playeremitter_ = std::make_unique<BaseEmitter>("Clear", playerspawndata_, playerrandom_);

	}

	void GameClearParticle::Update() {
		if (!player_) {
			return;
		}

		deltaTime = 1.0f / 60.0f;
		emitTimer_ += deltaTime;
		playeremitter_->SetTranslate(player_->GetTranslate());

		// プレイヤーの後方へ少しずらす
		Vector3 pos = player_->GetTranslate();
		Vector3 forward = player_->GetForward();
		playeremitter_->SetTranslate(pos - forward * 0.8f + Vector3{ 0.0f,0.4f,0.0f });		

		if (emitTimer_ >= emitInterval_) {
			playeremitter_->Emit();
			emitTimer_ = 0.0f;
		}
	}
}