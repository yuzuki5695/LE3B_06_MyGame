#include "EnemyState.h"
#include <BaseCharacter.h>
#include <Input.h>
#include <Player.h>
#include <CameraManager.h>
#include <Easing.h>
#include <Enemy.h>

using namespace MyEngine;
using namespace Easing;

namespace MyGame {
    void EnemyIdle::Update(BaseCharacter& character) {
        timer_ += 1.0f / 60.0f;

        float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);

        float s = EaseOutBack(t);

        character.GetObject3d()->SetScale({ s, s, s });

        if (t >= 1.0f) {
            t = 1.0f;
            s = EaseOutBack(t);
            character.GetObject3d()->SetScale({ s, s, s });
            character.ChangeState(std::make_unique<EnemyAlive>());
        }
    }

    void EnemyAlive::Update(BaseCharacter& character) {

    }

    void EnemyDead::Update(BaseCharacter& character) {
        character.Destroy();
    }
}