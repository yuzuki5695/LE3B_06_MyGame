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
			std::unique_ptr<BaseEmitter> emitter;
			MyEngine::Vector3 targettranslate;
			float timer = 0.0f;
			bool enable = true;
			float emitInterval_ = 0.0f;
			float deltaTime = 0.0f;
			float nextEmitTime;
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
		std::array<PartEmitter, 2> emitters_;
		MyEngine::ParticleSpawnData playerSpawnData_{};
		MyEngine::ParticleSpawnRandom playerRandom_;

		PartEmitter emitter01_;
		PartEmitter emitter02_;
	public:
		// setter	
		void SetTarget(uint32_t index, const MyEngine::Vector3& translate);	


	};
}