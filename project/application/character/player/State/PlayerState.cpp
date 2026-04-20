#include "PlayerState.h"
#include <BaseCharacter.h>
#include <Input.h>
#include <Player.h>
#include <CameraManager.h>
#include <SceneManager.h>
#include <SceneName.h>

using namespace MyEngine;

namespace MyGame {
    void PlayerStateIdle::Update(BaseCharacter& character) {
        if (CameraManager::GetInstance()->GetCurrentBehaviorAs <GamePlayCamera>()) {
            character.ChangeState(std::make_unique<PlayerStateMove>());
        }
    }

    void PlayerStateMove::Update(BaseCharacter& character) {
        // 必要なコンポーネント
        Player* player = static_cast<Player*>(&character);
        PlayerMove* move = player->GetMove();
        PlayerReticle* reticle = player->GetReticle();
        PlayerAttack* attack = player->GetAttack();
        Sprite* sprite = player->GetSprite();
        Object3d* target = player->GetTarget();

        // 移動処理の実行
        move->Update(player->GetObject3d()->GetTransform(), 0.17f);
        // 入力でレティクル移動
        reticle->UpdateInput();
        // スプライト位置更新
        sprite->SetPosition(reticle->GetScreenPos());
        // ワールド座標に変換
        Vector3 worldPos = reticle->ScreenToWorld(reticle->GetScreenPos(), CameraManager::GetInstance()->GetActiveCamera());
        player->SetAimWorldPos(worldPos);
        target->SetTranslate(player->GetAimWorldPos());
        attack->Update(player->GetObject3d()->GetTransform(), player->GetAimWorldPos());
    }

    void PlayerStateDead::Update(BaseCharacter& character) {
        Player* player = static_cast<Player*>(&character);
        PlayerDeath* death = player->GetDeath();
        Object3d* object = player->GetObject3d();
        death->Update(object->GetTransform().rotate, object->GetTransform().translate, object);
        player->SetActive(false);

        //    character.Destroy();
    }
}