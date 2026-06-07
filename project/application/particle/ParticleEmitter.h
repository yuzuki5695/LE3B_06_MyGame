#pragma once
#include <Vector3.h>
#include <string>
#include <vector>
#include <Transform.h>
#include <ParticleManager.h>

namespace MyGame {
	// パーティクル発生器
	class ParticleEmitter {
	public:

		ParticleEmitter(const std::string& name, const MyEngine::ParticleSpawnData& spawnData, float emitInterval);

		// 更新処理
		void Update();
		// パーティクル発生
		void Emit();
	private:
		// 名前
		std::string name_;
		// 発生データ
		MyEngine::ParticleSpawnData spawnData_;
		// 発生間隔
		float emitInterval_ = 0.1f;
		// タイマー
		float emitTimer_ = 0.0f;
		// 自動発生
		bool isAutoEmit_ = true;
	public:
	
	};
}