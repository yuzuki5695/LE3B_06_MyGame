#include "PlayerState.h"
#include <BaseCharacter.h>
#include <Input.h>
#include <Player.h>
#include <CameraManager.h>

using namespace MyEngine;
using namespace MyGame;

void PlayerStateIdle::Update(BaseCharacter& character) {
    //if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
        character.ChangeState(std::make_unique<PlayerStateMove>());
    //}
    CameraManager::GetInstance()->SetTarget(character.GetObject3d());
}

void PlayerStateMove::Update(BaseCharacter& character) {
    CameraManager::GetInstance()->SetTarget(character.GetObject3d());
    
    // BaseCharacterをPlayerにキャスト
    Player* player = static_cast<Player*>(&character);
    
    // 1. 移動に必要なコンポーネントとTransformを取得
    PlayerMove* move = player->GetMoveComponent();
    Object3d* object = player->GetObject3d();

    if (move && object) {
        // 2. 移動処理の実行
        move->Update(object->GetTransform(), 0.17f);
    }
}

void PlayerStateDead::Update(BaseCharacter& character) {
	character.Destroy();
}