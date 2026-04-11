#include "EnemyState.h"
#include <BaseCharacter.h>
#include <Input.h>
#include <Player.h>
#include <CameraManager.h>

using namespace MyEngine;

namespace MyGame {
    void EnemyIdle::Update(BaseCharacter& character) {
    
    }

    void EnemyAlive::Update(BaseCharacter& character) {

    }

    void EnemyDead::Update(BaseCharacter& character) {
        character.Destroy();
    }
}