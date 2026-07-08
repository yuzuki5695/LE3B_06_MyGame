#pragma once
#include <SceneParticleBase.h>
#include <Player.h>

namespace MyGame {	

    /// <summary>
	/// ゲームクリアシーン用のパーティクルを管理するクラス
    /// </summary>
	class GameClearParticle : public SceneParticleBase {
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
		Player* player_;              // プレイヤーのポインタ	
		float deltaTime;
	public:
        // setter
        void SetPlayer(Player* player) { player_ = player; }	
	};
}