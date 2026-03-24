#pragma once
class Player;

class IPlayerState {
public:
    virtual ~IPlayerState() = default;
    virtual void Enter(Player* p) {} // 状態開始時
    virtual void Update(Player* p) = 0;
    virtual void Exit(Player* p) {}  // 状態終了時
};