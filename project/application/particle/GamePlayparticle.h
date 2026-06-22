#pragma once
#include <SceneParticleBase.h>

namespace MyGame {

    class Player;

    class GamePlayParticle : public SceneParticleBase {
    public:
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

        void AddExplosion(const MyEngine::Vector3& pos, uint32_t exp);
        void EmitPlayerTrail();

    private: // メンバ変数
        std::vector<MyEngine::Vector3> explosionQueue_;

        std::unique_ptr<BaseEmitter> explosionEmitter_;
        MyEngine::ParticleSpawnData explosionspawndata_{};
        MyEngine::ParticleSpawnRandom explosionRandom_;

        std::unique_ptr<BaseEmitter> playeremitter_;
        MyEngine::ParticleSpawnData  playerspawndata_{};
        MyEngine::ParticleSpawnRandom  playerrandom_;
        float playerEmitTimer_;
        float playerEmitInterval_;
        Player* player_;
        std::vector<ExpOrb> expOrbs_;

    public:
        void SetPlayer(Player* player) { player_ = player; }	
    };
}