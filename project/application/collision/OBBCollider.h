#pragma once
#include <Collider.h>
#include <CollisionUtils.h>
#include <Object3d.h>
#include <ICollisionReceiver.h>

namespace MyGame {
    /// <summary>
	/// OBBコライダクラス
    /// </summary>
    class OBBCollider : public Collider {
	public: // メンバ関数
        /// <summary>
		/// コンストラクタ
        /// </summary>
        /// <param name="object"></param>
        OBBCollider(MyEngine::Object3d* object) : object_(object) {}
        /// <summary>
		/// OBBを取得する関数 
        /// </summary>
        /// <returns></returns>
        MyEngine::OBB GetOBB() const override { return CollisionUtils::CreateOBB(object_); }
        /// <summary>
		/// 衝突したときの処理（仮想関数）S
        /// </summary>
        /// <param name="other"></param>
        /// <param name="mtv"></param>
        void DispatchCollision(Collider* other) override {
            if (owner_) {
                auto* receiver = static_cast<ICollisionReceiver*>(owner_);
                receiver->OnCollision(other);
            }
        }
    private: // メンバ変数
		MyEngine::Object3d* object_; // OBBを生成するためのObject3dへのポインタ
    };
}