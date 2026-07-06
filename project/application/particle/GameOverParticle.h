#pragma once
#include <SceneParticleBase.h>
#include <Object3d.h>

namespace MyGame {

	/// <summary>
	/// ゲームオーバーシーン用のパーティクルを管理するクラス
	/// </summary>
	class GameOverParticle : public SceneParticleBase {
	public:
		struct PartEmitter {
			MyEngine::Object3d* target = nullptr;
			std::unique_ptr<BaseEmitter> emitter;
			float timer = 0.0f;
			bool enable = true;
		};
	public: // メンバ関数
		/// <summary>
		/// 初期化処理 
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// 更新処理
		/// </summary>
		void Update() override;
	private: // メンバ変数
		std::vector<PartEmitter> emitters_;
		MyEngine::ParticleSpawnData playerSpawnData_{};
		MyEngine::ParticleSpawnRandom playerRandom_;

		float emitInterval_ = 0.03f;
	public:
		// setter	
		void SetTargets(const std::vector<MyEngine::Object3d*>& parts);
	};
}