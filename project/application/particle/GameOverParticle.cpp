#include "GameOverParticle.h"
#include <Player.h>
#include <MatrixVector.h>
#include <CameraManager.h>
#include <SceneEmitterManager.h>

using namespace MyEngine::MatrixVector;

using namespace MyEngine;

namespace MyGame {

	void GameOverParticle::Initialize() {
		// パーティクルグループの作成
		ParticleManager::GetInstance()->CreateParticleGroup("Parts", "Models/Particles/Particle_02.png", "Models/Particles/Particle_02.obj", 2000);

		//--------------------------------------
		// 設定
		//--------------------------------------
		playerSpawnData_.transform.scale = { 0.08f,0.08f,0.08f };
		playerSpawnData_.count = 5;
		playerSpawnData_.behavior = ParticleBehavior::Normal;
		playerSpawnData_.lifetime = 1.0f;
		playerRandom_.translate = { {-0.2f,-0.2f,-0.2f}, { 0.2f, 0.2f, 0.2f}, true };
		playerRandom_.velocityTranslate = { {-0.01f,0.05f,-0.01f}, { 0.01f,0.12f, 0.01f}, true };
		playerRandom_.velocityScale = { {0.02f,0.02f,0.02f}, {0.04f,0.04f,0.04f}, true };
		playerRandom_.lifetime = { 0.4f, 0.8f, true };
		playerRandom_.count = { 5, 8, true };
	}

	void GameOverParticle::Update() {
		constexpr float deltaTime = 1.0f / 60.0f;
		for (auto& emitter : emitters_) {
			if (!emitter.target) {
				continue;
			}			
			if (!emitter.enable) {
				continue;
			}
			// パーツが一定以下まで落ちたら煙を出さない
			if (emitter.target->GetTranslate().y <= -10.0f) {
				emitter.enable = false;
				continue;
			}

			emitter.timer += deltaTime;
			emitter.emitter->SetTranslate(emitter.target->GetTranslate());

			if (emitter.timer >= emitInterval_) {
				emitter.emitter->Emit();
				emitter.timer = 0.0f;
			}
		}
	}

	void GameOverParticle::SetTargets(const std::vector<Object3d*>& parts) {
		emitters_.clear();
		for (Object3d* part : parts) {
			PartEmitter emitter;
			emitter.target = part;
			emitter.emitter = std::make_unique<BaseEmitter>("Parts", playerSpawnData_, playerRandom_);
			emitter.timer = 0.0f;
			emitters_.push_back(std::move(emitter));
		}
	}
}