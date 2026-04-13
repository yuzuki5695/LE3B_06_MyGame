#pragma once
#include <ICharacterState.h>

namespace MyGame {
    // 待機状態
    class EnemyIdle : public ICharacterState {
    public:
        void Update(BaseCharacter& character) override;
    private:
        float timer_ = 0.0f;
        float duration_ = 1.0f; // 出現時間
    };

    // 生きている状態
    class EnemyAlive : public ICharacterState {
    public:
        void Update(BaseCharacter& character) override;
    };

    // 死亡状態
    class EnemyDead : public ICharacterState {
    public:
        void Update(BaseCharacter& character) override;
    };
}