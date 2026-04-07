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

    // 必要なコンポーネント
    PlayerMove* move = player->GetMove();
    PlayerReticle* reticle = player->GetReticle();
    Object3d* object = player->GetObject3d();
    Sprite* sprite = player->GetSprite();     
    Object3d* target = player->GetTarget();

    if (move && object) {
        // 2. 移動処理の実行
        move->Update(object->GetTransform(), 0.17f);

        // ① 入力でレティクル移動
        reticle->UpdateInput();

        // ② スプライト位置更新
        sprite->SetPosition(reticle->GetScreenPos());

        // ③ ワールド座標に変換
        Vector3 worldPos = reticle->ScreenToWorld(reticle->GetScreenPos(), CameraManager::GetInstance()->GetActiveCamera());

        // ④ 3Dターゲットに反映
        target->SetTranslate(worldPos);
    }
}

void PlayerStateDead::Update(BaseCharacter& character) {
	character.Destroy();
}