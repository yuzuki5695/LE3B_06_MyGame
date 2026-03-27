#pragma once
#include <EnemyData.h>

class EnemyWeapon{
public:// メンバ関数  

    void Initialize(float interval);
    void Update(const Vector3& enemyPos, const Vector3& playerPos);
private: // メンバ変数
    float bulletTimer_ = 0.0f;
    float bulletInterval_ = 1.0f;
};