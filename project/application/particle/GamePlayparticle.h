#pragma once
#include <SceneParticleBase.h>

namespace MyGame {
 
    class Player;

    class GamePlayParticle : public SceneParticleBase {
    public:
        void Initialize() override;
        void Update() override;

    private:
        std::vector<MyEngine::Vector3> explosionQueue_;

        std::unique_ptr<BaseEmitter> explosionEmitter_;
		MyEngine::ParticleSpawnData explosionspawndata_{};
        MyEngine::ParticleSpawnRandom explosionRandom_;
     
        //std::unique_ptr<BaseEmitter> shockwaveEmitter_;
        //MyEngine::ParticleSpawnData  shockwavedata_{};
        //MyEngine::ParticleSpawnRandom shockwaveRandom_;

    public:
        void AddExplosion(const MyEngine::Vector3& pos);
    };
}