#pragma once
#include <ICharacterState.h>

namespace MyGame {
    // 待機状態
    class PlayerStateIdle : public ICharacterState {
    public:
        void Update(BaseCharacter& character) override;
    };

    // 移動状態
    class PlayerStateMove : public ICharacterState {
    public:
        void Update(BaseCharacter& character) override;
    };

    // 死亡状態
    class PlayerStateDead : public ICharacterState {
    public:
        void Update(BaseCharacter& character) override;
    };
}