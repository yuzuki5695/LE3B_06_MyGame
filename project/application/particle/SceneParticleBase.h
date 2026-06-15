#pragma once
#include <memory>
#include <vector>
#include <BaseEmitter.h>

namespace MyGame {

    class SceneParticleBase {
    public:
        virtual ~SceneParticleBase() = default;

        virtual void Initialize() = 0;
        virtual void Update() = 0;

    };
}