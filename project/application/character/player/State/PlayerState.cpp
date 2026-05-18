#include "PlayerState.h"
#include <BaseCharacter.h>
#include <Input.h>
#include <Player.h>
#include <CameraManager.h>
#include <SceneManager.h>
#include <SceneName.h>
#include <FadeManager.h>

using namespace MyEngine;

namespace MyGame {
    void PlayerStateIdle::Update(BaseCharacter& character) {
		// 現在のシーンはゲームプレイなら移動状態へ遷移
        if (CameraManager::GetInstance()->GetCurrentBehaviorAs <GamePlayCamera>()) {
            character.ChangeState(std::make_unique<PlayerStateMove>());
        }
    }

    void PlayerStateMove::Update(BaseCharacter& character) {
        // 必要なコンポーネント
        Player* player = dynamic_cast<Player*>(&character);
        if (Input::GetInstance()->PushKey(DIK_Z)) {
            character.ChangeState(std::make_unique<PlayerStateDead>());
        }

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
        if (player->IsActive()) {
			player->SetActive(false); // アクティブフラグを下げる
			player->Destroy();        // 削除予約
            // 当たり判定の登録解除
            CollisionManager::GetInstance()->UnregisterCollider(player->GetCollider());
            // カメラの状態をロックオンに変更(ゲームプレイカメラの状態なので向こうでサブカメラに切り替わる)
            CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()->SetCameraState(CameraDefs::CameraState::LockOn);
        }
        // 死亡演出の更新
        player->GetDeath()->Update(player->GetObject3d()->GetTransform().rotate, player->GetObject3d()->GetTransform().translate, player->GetObject3d());
        // ゲームオーバーシーンへ遷移
        FadeManager::GetInstance()->SceneChangeFade(SceneName::GAMEOVER, FadeStyle::Normal, 1.5f);
    }
}