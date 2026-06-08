#pragma once
#include <ParticleEmitter.h>
#include <Object3d.h>

namespace MyGame {
    class GamePlayparticle {
    public:
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize(MyEngine::Object3d* target);
        /// <summary>
        /// 毎フレームの更新処理
        /// </summary>
        void Update();


    private:;
        std::unique_ptr<ParticleEmitter> particleEmitter_;
        MyEngine::ParticleSpawnRandom random_;
    };
}