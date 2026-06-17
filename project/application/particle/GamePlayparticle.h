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

        //std::vector<std::unique_ptr<ExperienceParticle>> expParticles_;
        //MyEngine::Vector3 targetpos_;

        //MyEngine::ParticleSpawnRandom shockwaveRandom_;
        //std::unique_ptr<BaseEmitter> shockwaveEmitter_;
    public:
//        void AddExplosion(const MyEngine::Vector3& pos) { explosionQueue_.push_back(pos); }
        void AddExplosion(const MyEngine::Vector3& pos);
        //void SetTarGetPos(const MyEngine::Vector3& targetpos) { targetpos_ = targetpos; }
    };
}