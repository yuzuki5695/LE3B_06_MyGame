#pragma once
#include <SceneParticleBase.h>

namespace MyGame {
	// 前方宣言
    class Player;    
    /// <summary>
	/// ゲームプレイシーン用のパーティクルを管理するクラス
    /// </summary>
    class GamePlayParticle : public SceneParticleBase {
    public: // メンバ関数       
		// 爆発時に出現する経験値オーブの情報を保持する構造体
        struct ExpOrb {
            MyEngine::Vector3 position;
            uint32_t exp;
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
        /// <summary>
		/// 爆発エフェクトの追加
        /// </summary>
        /// <param name="pos">爆発の位置</param>
		/// <param name="exp">爆発で得られる経験値</param>
        void AddExplosion(const MyEngine::Vector3& pos, uint32_t exp);
        /// <summary>
		/// プレイヤーの移動に追従するパーティクルを発生させる
        /// </summary>
        void EmitPlayerTrail();
    private: // メンバ変数
		std::vector<MyEngine::Vector3> explosionQueue_; // 爆発エフェクトの発生位置を保持するキュー
        ///-----------------------------------------///
        ///-------------爆発エフェクト----------------///
        ///-----------------------------------------///
		std::unique_ptr<BaseEmitter> explosionEmitter_;    // 爆発エフェクト用のパーティクルエミッター
		MyEngine::ParticleSpawnData explosionspawndata_{}; // 爆発エフェクトのパーティクル発生データ
		MyEngine::ParticleSpawnRandom explosionRandom_;    // 爆発エフェクトのパーティクル発生ランダムデータ
        ///-----------------------------------------///
        ///-----------プレイヤ―エフェクト--------------///
        ///-----------------------------------------///
		std::unique_ptr<BaseEmitter> playeremitter_;    // プレイヤーの移動に追従するパーティクルエミッター
		MyEngine::ParticleSpawnData  playerspawndata_{}; // プレイヤーの移動に追従するパーティクル発生データ
		MyEngine::ParticleSpawnRandom  playerrandom_;    // プレイヤーの移動に追従するパーティクル発生ランダムデータ
		float playerEmitTimer_;       // プレイヤーの移動に追従するパーティクル発生タイマー
		float playerEmitInterval_;    // プレイヤーの移動に追従するパーティクル発生間隔
		Player* player_;              // プレイヤーのポインタ
		std::vector<ExpOrb> expOrbs_; // 爆発エフェクトで得られる経験値オーブの情報を保持するベクター
    public:
        // setter
        void SetPlayer(Player* player) { player_ = player; }	
    };
}