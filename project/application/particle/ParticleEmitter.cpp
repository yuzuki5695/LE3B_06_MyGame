#include "ParticleEmitter.h"
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace MyEngine;

namespace MyGame {


	ParticleEmitter::ParticleEmitter(const std::string& name, const MyEngine::ParticleSpawnData& spawnData, const MyEngine::ParticleSpawnRandom& random, float emitInterval) {
		name_ = name;
		spawnData_ = spawnData;
		random_ = random;
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
		auto& pm = *ParticleManager::GetInstance();

		uint32_t emitCount = spawnData_.count;

		// 発生数のランダム適用
		if (random_.count.enable) {
			// 💡 引数を明示的に float にキャストして pm.Rand に渡します
			emitCount = static_cast<uint32_t>(pm.Rand(static_cast<float>(random_.count.min), static_cast<float>(random_.count.max)));
		}

		emitCount = std::max(1u, emitCount);

		for (uint32_t i = 0; i < emitCount; i++) {
			ParticleSpawnData data = BuildSpawnData(pm);
			pm.Emit(name_, data);
		}
	}

	ParticleSpawnData ParticleEmitter::BuildSpawnData(ParticleManager& pm) {
		ParticleSpawnData data = spawnData_;

		// 各トランスフォームのランダム適用
		if (random_.translate.enable)
			data.transform.translate += pm.SafeRandVec3({ random_.translate.min, random_.translate.max });

		if (random_.rotate.enable)
			data.transform.rotate += pm.SafeRandVec3({ random_.rotate.min, random_.rotate.max });

		if (random_.scale.enable)
			data.transform.scale += pm.SafeRandVec3({ random_.scale.min, random_.scale.max });
		
		// 速度のランダム適用
		if (random_.velocityTranslate.enable) {	
			data.velocity.translate += pm.SafeRandVec3(random_.velocityTranslate);
		}
		if (random_.velocityRotate.enable) {
			data.velocity.rotate += pm.SafeRandVec3(random_.velocityRotate);
		}
		if (random_.velocityScale.enable) {
			data.velocity.scale += pm.SafeRandVec3(random_.velocityScale);
		}

		// カラーのランダム適用
		if (random_.color.enable) {
			data.color.x = pm.Rand(random_.color.min.x, random_.color.max.x);
			data.color.y = pm.Rand(random_.color.min.y, random_.color.max.y);
			data.color.z = pm.Rand(random_.color.min.z, random_.color.max.z);
			data.color.w = pm.Rand(random_.color.min.w, random_.color.max.w);
		}

		// 寿命のランダム適用
		if (random_.lifetime.enable)
			data.lifetime = pm.Rand(random_.lifetime.min, random_.lifetime.max);

		return data;
	}
}