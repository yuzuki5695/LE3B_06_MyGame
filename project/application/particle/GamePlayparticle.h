#pragma once
#include <SceneParticleBase.h>

namespace MyGame {
 
    class GamePlayParticle : public SceneParticleBase {
    public:
        void Initialize() override;
        void Update() override;

    private:
        std::vector<MyEngine::Vector3> explosionQueue_;

        std::unique_ptr<BaseEmitter> explosionEmitter_;
		MyEngine::ParticleSpawnData explosionspawndata_{};
        MyEngine::ParticleSpawnRandom explosionRandom_;

        //MyEngine::ParticleSpawnRandom shockwaveRandom_;
        //std::unique_ptr<BaseEmitter> shockwaveEmitter_;
    public:
        void AddExplosion(const MyEngine::Vector3& pos) { explosionQueue_.push_back(pos); }
    };
}