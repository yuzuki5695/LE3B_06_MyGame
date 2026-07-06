#include "TitleParticle.h"
#include <MatrixVector.h>
#include <CameraManager.h>
#include <SceneEmitterManager.h>

using namespace MyEngine::MatrixVector;

using namespace MyEngine;

namespace MyGame {

	void TitleParticle::Initialize() {
		// パーティクルグループの作成
		ParticleManager::GetInstance()->CreateParticleGroup("Title", "Models/Particles/Particle_02.png", "Models/Particles/Particle_02.obj", 1000);

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
		// パーティクルの生成
		playeremitter_ = std::make_unique<BaseEmitter>("Title", playerspawndata_, playerrandom_);
	}

	void TitleParticle::Update() {
		if (!object_) {
			return;
		}

		constexpr float deltaTime = 1.0f / 60.0f;
		emitTimer_ += deltaTime;
		playeremitter_->SetTranslate(object_->GetTranslate());

		// プレイヤーの後方へ少しずらす
		Vector3 pos = object_->GetTranslate();
		playeremitter_->SetTranslate({ pos.x, pos.y + 0.4f, pos.z - 0.6f });

		if (emitTimer_ >= emitInterval_) {
			playeremitter_->Emit();
			emitTimer_ = 0.0f;
		}
	}
}