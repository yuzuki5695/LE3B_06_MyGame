#pragma once
#include <SceneParticleBase.h>

namespace MyGame {
 
    class Player;

    class GamePlayParticle : public SceneParticleBase {
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

    public:
        void AddExplosion(const MyEngine::Vector3& pos);
    };
}