#pragma once
#include <Collider.h>

namespace MyGame {
    /// <summary>
	/// 衝突を受け取る側のインターフェースクラス
    /// </summary>
    class ICollisionReceiver {
    public:
        virtual ~ICollisionReceiver() = default;
        virtual void OnCollision(Collider* other) = 0;
    };

}