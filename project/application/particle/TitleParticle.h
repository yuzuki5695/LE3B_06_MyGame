#pragma once
#include <SceneParticleBase.h>
#include <Object3d.h>

namespace MyGame {	

    /// <summary>
	/// タイトルシーン用のパーティクルを管理するクラス
    /// </summary>
	class TitleParticle : public SceneParticleBase {
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
		///-----------------------------------------///
		///-----------プレイヤ―エフェクト--------------///
		///-----------------------------------------///
		std::unique_ptr<BaseEmitter> playeremitter_;    // プレイヤーの移動に追従するパーティクルエミッター
		MyEngine::ParticleSpawnData  playerspawndata_{}; // プレイヤーの移動に追従するパーティクル発生データ
		MyEngine::ParticleSpawnRandom  playerrandom_;    // プレイヤーの移動に追従するパーティクル発生ランダムデータ
		float emitTimer_;
		float emitInterval_;
		MyEngine::Object3d* object_;              // プレイヤーのポインタ
	public:
		// setter
		void SetObject3d(MyEngine::Object3d* object) { object_ = object; }
	};
}