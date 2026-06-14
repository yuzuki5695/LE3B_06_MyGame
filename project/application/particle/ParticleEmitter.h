#pragma once
#include <Vector3.h>
#include <string>
#include <vector>
#include <Transform.h>
#include <ParticleManager.h>
#include <random>

namespace MyGame {
	// パーティクル発生器
	class ParticleEmitter {
	public:

		ParticleEmitter(const std::string& name, const MyEngine::ParticleSpawnData& spawnData, const MyEngine::ParticleSpawnRandom& random, float emitInterval);

		// 更新処理
		void Update();
		// パーティクル発生
		void Emit();
		void SetInterval(float interval) { emitInterval_ = interval; }

		MyEngine::ParticleSpawnData BuildSpawnData(MyEngine::ParticleManager& pm);
	private:
		// 名前
		std::string name_;
		// 発生データ
		MyEngine::ParticleSpawnData spawnData_;
		std::mt19937 rng_;
		MyEngine::ParticleSpawnRandom random_;
		// 発生間隔
		float emitInterval_ = 0.1f;
		// タイマー
		float emitTimer_ = 0.0f;
		// 自動発生
		bool isAutoEmit_ = true;
	public:

	};
}