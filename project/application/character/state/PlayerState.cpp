#include "PlayerState.h"
#include <BaseCharacter.h>
#include <Input.h>
#include <Player.h>

using namespace MyEngine;
using namespace MyGame;

void PlayerStateIdle::Update(BaseCharacter& character) {
    if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
        character.ChangeState(std::make_unique<PlayerStateMove>());
    }
}

void PlayerStateMove::Update(BaseCharacter& character) {
    // BaseCharacterをPlayerにキャスト
    // (実行時に型が確定している場合は static_cast を使用)
    Player* player = static_cast<Player*>(&character);
    
    // 1. 移動に必要なコンポーネントとTransformを取得
    PlayerMove* move = player->GetMoveComponent();
    Object3d* object = player->GetObject3d();

    if (move && object) {
        // 2. 移動処理の実行 [#Tag:Functions]
        move->Update(object->GetTransform(), 0.1f);
    }
}

void PlayerStateDead::Update(BaseCharacter& character) {
	character.Destroy();
}