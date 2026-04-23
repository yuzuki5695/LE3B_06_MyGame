#pragma once
#include <Object3d.h>                   

namespace MyGame {
    /// <summary>
    /// 敵の攻撃クラス
    /// </summary>
    class EnemyAttack {
    public:

        void Initialize();
        void Update(const MyEngine::Transform& transform, const MyEngine::Vector3& aimWorldPos);

    private:
        float coolTime_ = 0.2f;   // 発射間隔
        float timer_ = 0.0f;
       // Enemy* owner_ = nullptr;
        float interval_; // 1秒ごとに発射
    public:
        //void SetOwner(Enemy* owner) { owner_ = owner; }
    };
}