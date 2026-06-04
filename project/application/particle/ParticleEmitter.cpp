#include "ParticleEmitter.h"
#include "TextureManager.h"
#include <ParticleManager.h>
#include <cmath>

using namespace MyEngine;

namespace MyGame {

	ParticleEmitter::ParticleEmitter(const std::string& name, const uint32_t count, const Transform& transform, const Vector4& color, const float lifetime, const float currentTime, const Velocity& Velocity) {
		name_ = name;//名前
		this->count = count;//count
		transform_ = transform;//位置
		this->color = color; // カラー
		frequency = lifetime;//寿命
		frequencyTime = currentTime;//現在の寿命
		velocity_ = Velocity; // 風の強さ
	}

	void ParticleEmitter::Update() {
		// 時間を進める
		frequencyTime += 1.0f / 60.0f;

		// isAutoEmit_ が true のとき、自動発生処理
		if (isAutoEmit_) {
			// 発生間隔を超えたら Emit 実行
			if (frequencyTime >= frequency) {
				frequencyTime = 0.0f; // リセット
				Emit(); // パーティクル発生
			}
		}
	}

	void ParticleEmitter::Emit() {
		//パーティクルを発生
		ParticleManager::GetInstance()->Emit(name_, transform_, color, count, velocity_, frequency);
	}
}