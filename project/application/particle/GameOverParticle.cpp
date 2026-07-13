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
		playerSpawnData_.count = 1;
		playerSpawnData_.behavior = ParticleBehavior::Normal;
		playerSpawnData_.lifetime = 1.0f;
		playerRandom_.translate = { {-0.2f,-0.2f,-0.2f}, { 0.2f, 0.2f, 0.2f}, true };
		playerRandom_.velocityTranslate = { {-0.01f,0.05f,-0.01f}, { 0.01f,0.12f, 0.01f}, true };
		playerRandom_.velocityScale = { {0.02f,0.02f,0.02f}, {0.04f,0.04f,0.04f}, true };
		playerRandom_.lifetime = { 0.4f, 0.8f, true };
		playerRandom_.count = { 5, 10, true };


		emitter01_.emitter = std::make_unique<BaseEmitter>("Parts", playerSpawnData_, playerRandom_);
		emitter01_.timer = 0.0f;
		emitter01_.enable = true;
		emitter01_.emitInterval_ = 1.5f;
		emitter01_.deltaTime = 0.0f;
		emitter01_.targettranslate = { 0.0f,0.0f,50.0f };

		emitter02_.emitter = std::make_unique<BaseEmitter>("Parts", playerSpawnData_, playerRandom_);
		emitter02_.timer = 0.0f;
		emitter02_.enable = true;
		emitter02_.emitInterval_ = 0.5f;
		emitter02_.deltaTime = 0.0f;
		emitter02_.targettranslate = { -3.0f,0.0f,30.0f };
	}

	void GameOverParticle::Update() {
		emitter01_.deltaTime = 1.0f / 60.0f;

		if (emitter01_.enable) {
			emitter01_.timer += emitter01_.deltaTime;
			if (emitter01_.timer >= emitter01_.emitInterval_) {			
				emitter01_.emitter->SetTranslate(emitter01_.targettranslate);
				emitter01_.emitter->Emit();
				emitter01_.timer = 0.0f;
			}
		}

		emitter02_.deltaTime = 1.0f / 60.0f;	
		if (emitter02_.enable) {
			emitter02_.timer += emitter02_.deltaTime;
			// エミッターにパーツの座標を追従させる		
			if (emitter02_.timer >= emitter02_.emitInterval_) {
				emitter02_.emitter->SetTranslate(emitter02_.targettranslate);
				emitter02_.emitter->Emit();
				emitter02_.timer = 0.0f;
			}
		}
	}	

	void GameOverParticle::SetTarget(uint32_t index, const MyEngine::Vector3& translate) {
		if (index == 0) {
			emitter01_.targettranslate = translate;
		} else if (index == 1) {
			emitter02_.targettranslate = translate;
		}
	}

	//void GameOverParticle::Update() {
	//	deltaTime = 1.0f / 60.0f;

	//	for (auto& emitter : emitters_) {
	//		if (emitter.targettranslate.y <= -10.0f) {
	//			emitter.enable = false;
	//			continue;
	//		}

	//		if (emitter.enable) {
	//			emitter.timer += deltaTime;
	//			// エミッターにパーツの座標を追従させる
	//			emitter.emitter->SetTranslate(emitter.targettranslate);
	//			if (emitter.timer >= emitter.emitInterval_) {
	//				emitter.emitter->Emit();
	//				emitter.timer = 0.0f;
	//			}
	//		}
	//	}
	//}	
		

	//void GameOverParticle::SetTarget(uint32_t index, const MyEngine::Vector3& translate) {
	//	if (index >= emitters_.size()) {
	//		return;
	//	}
	//	emitters_[index].targettranslate = translate;
	//}
}