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
    public:
        void Initialize() override;
        void Update() override;

        void EmitPlayerTrail();

    private:
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
        //        void AddExplosion(const MyEngine::Vector3& pos);
        void AddExplosion(const MyEngine::Vector3& pos, uint32_t exp);

        void SetPlayer(Player* player) { player_ = player; }
    };
}