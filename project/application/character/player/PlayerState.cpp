#include "PlayerState.h"
#include <Player.h>
#include <CameraManager.h>

// 通常状態
void PlayerStateAlive::Update(Player* player) {
    CameraManager* camMgr = CameraManager::GetInstance();
    // 1. 今まで Player::UpdateAlive() に書いていた処理を呼ぶ
    // 移動処理の更新
    player->GetMove()->Update(player->GetTransform(), camMgr->GetMainCamera()->GetRotate());
    // レティクル
    player->GetReticle()->Update(player->GetTarGetTransform(), player->GetTransform().translate, player->GetTarget());
    // 攻撃
    player->GetWeapon()->Update(player->GetTransform().translate, player->GetTarget()->GetTranslate(), camMgr->GetGameplayCamera());
    // レティクル(2Dスプライト)の同期 
    player->GetReticle()->UpdateSprite(player->GetTarget()->GetTranslate(), player->GetTargetReticle(), camMgr->GetActiveCamera());
}

// 死亡状態
void PlayerStateDead::Update(Player* player) {
    // 死亡演出：ここに落下や回転のロジックを書く（後にクラス化も可能）
    player->GetDeath()->Update(player->GetRotate(), player->GetDeathOffset(), player->GetPlayerObject());
    // 座標の同期（これが重要）
   // player->SyncWorldTransformByRail();
}