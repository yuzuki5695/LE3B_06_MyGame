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
        Player* player = dynamic_cast<Player*>(&character);

        // アクティブ中は各更新処理を行う
        if (character.IsActive()) {
            // 移動処理の実行
            player->GetMove()->Update(player->GetObject3d()->GetTransform(), 0.17f);
            // 入力でレティクル移動
            player->GetReticle()->UpdateInput();
            // スプライト位置更新
            player->GetSprite()->SetPosition(player->GetReticle()->GetScreenPos());
            // ワールド座標に変換
            Vector3 worldPos = player->GetReticle()->ScreenToWorld(player->GetReticle()->GetScreenPos(), CameraManager::GetInstance()->GetActiveCamera());
            player->SetAimWorldPos(worldPos);
            player->GetTarget()->SetTranslate(player->GetAimWorldPos());
            player->GetAttack()->Update(player->GetObject3d()->GetTransform(), player->GetAimWorldPos());
        }
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