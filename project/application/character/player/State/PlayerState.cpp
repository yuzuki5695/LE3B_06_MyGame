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
    ///====================================================
    /// 更新処理
    ///====================================================
    void PlayerStateIdle::Update(BaseCharacter& character) {
		// 現在のシーンはゲームプレイなら移動状態へ遷移
        if (CameraManager::GetInstance()->GetCurrentBehaviorAs <GamePlayCamera>()) {
            character.ChangeState(std::make_unique<PlayerStateMove>());
        }
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void PlayerStateMove::Update(BaseCharacter& character) {
        // 必要なコンポーネント
        Player* player = dynamic_cast<Player*>(&character); 

        // アクティブ中は各更新処理を行う
        if (character.IsActive()) {
            // 移動処理の実行
            player->GetMove()->Update(player->GetObject3d()->GetTransform(), 0.17f);
            // プレイヤー画面座標
            Vector2 playerScreenPos = { 640.0f + player->GetMove()->GetRelativePos().x, 360.0f + player->GetMove()->GetRelativePos().y };
            // レティクル更新
            player->GetReticle()->Update();
            // 攻撃の更新
            player->GetAttack()->Update(player->GetObject3d()->GetTransform(), player->GetTarget()->GetTranslate(), player->GetLevel());
        }
    }
    ///====================================================
    /// 更新処理
    ///====================================================
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