#pragma once
#include <Object3d.h>

namespace MyGame {
    /// <summary>
    /// プレイヤーの攻撃クラス
    /// </summary>
    class PlayerAttack {
    public:

        void Initialize();
        void Update(const MyEngine::Transform& playerTransform, const MyEngine::Vector3& aimWorldPos);

    private:
        float coolTime_ = 0.2f;   // 発射間隔
        float timer_ = 0.0f;
    };
}