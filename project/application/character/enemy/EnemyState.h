#pragma once
#include <IEnemyState.h>

// 前方宣言
class Enemy;

class EnemyStateActive : public IEnemyState {
public:
    void Update(Enemy* enemy) override;
};

class EnemyStateSpawn : public IEnemyState {
private:
    float timer_ = 0.0f;
    float duration_ = 2.0f;
public:
    void Enter(Enemy* enemy) override;
    void Update(Enemy* enemy) override;
};

class EnemyStateWait : public IEnemyState {
public:
    void Update(Enemy* enemy) override;
};

class EnemyStateDead : public IEnemyState {
public:
    void Update(Enemy* enemy) override;
};
