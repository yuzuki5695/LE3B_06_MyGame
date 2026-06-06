#include "ParticleEmitter.h"
#include "TextureManager.h"
#include <ParticleManager.h>
#include <cmath>

using namespace MyEngine;

namespace MyGame {

	ParticleEmitter::ParticleEmitter(const std::string& name, const Transform& transform, const Vector4& color, const uint32_t count, const Velocity& Velocity, const float frequency, const float lifetime) {
		this->name_ = name;//名前
		this->transform_ = transform;//位置
		this->count = count; // カラー
		this->color = color; // count
		this->velocity_ = Velocity; // 風の強さ
		this->frequency = frequency;    //寿命
		this->frequencyTime = lifetime;//現在の寿命
		isAutoEmit_ = true;
		emitTimer_ = 0.0f;
	}

	void ParticleEmitter::Update() {
		// isAutoEmit_ が true のとき、自動発生処理
		if (isAutoEmit_) {
			// 時間を進める
			emitTimer_ += 1.0f / 60.0f;
			// 発生間隔を超えたら Emit 実行
			if (emitTimer_ >= frequency) {
				emitTimer_ = 0.0f; // リセット
				Emit(); // パーティクル発生
			}
		}
	}

	void ParticleEmitter::Emit() {
		//パーティクルを発生
		ParticleManager::GetInstance()->Emit(name_, transform_, color, count, velocity_, frequencyTime);
	}
}