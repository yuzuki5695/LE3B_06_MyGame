#include "CharacterState.h"
#include <BaseCharacter.h>

namespace MyGame {

    void CharacterState::ChangeState(BaseCharacter& character, std::unique_ptr<ICharacterState> nextState) {
        // 現在のステートが存在する場合、終了処理を呼び出す
        if (currentState_) {
            currentState_->Exit(character);
        }
        
        // 新しいステートへ所有権を移動し、開始処理を呼び出す
        currentState_ = std::move(nextState);
        
        if (currentState_) {
            currentState_->Enter(character);
        }
    }

    void CharacterState::Update(BaseCharacter& character) {
        // 現在のステートに更新処理を委譲
        if (currentState_) {
            currentState_->Update(character);
        }
    }
}