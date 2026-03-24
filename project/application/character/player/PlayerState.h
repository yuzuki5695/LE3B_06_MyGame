#pragma once
#include <IPlayerState.h>

// 待機状態
class PlayerStateAlive : public IPlayerState {
public:
    void Update(Player* player) override;
};

// 移動状態
class PlayerStateDead : public IPlayerState {
public:
    void Update(Player* player) override;
};