#include "ParticleEmitter.h"
#include "TextureManager.h"
#include <ParticleManager.h>
#include <cmath>

using namespace MyEngine;

namespace MyGame {

	ParticleEmitter::ParticleEmitter(const std::string& name, const ParticleSpawnData& spawnData, float emitInterval) {
		name_ = name;
		spawnData_ = spawnData;
		emitInterval_ = emitInterval;
		isAutoEmit_ = true;
		emitTimer_ = 0.0f;
	}

	void ParticleEmitter::Update() {
		if (!isAutoEmit_) {
			return;
		}

		emitTimer_ += 1.0f / 60.0f;

		if (emitTimer_ >= emitInterval_) {

			emitTimer_ = 0.0f;

			Emit();
		}
	}

	void ParticleEmitter::Emit() {
		//パーティクルを発生
		ParticleManager::GetInstance()->Emit(name_, spawnData_);
	}
}