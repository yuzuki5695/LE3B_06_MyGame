#pragma once
#include <vector>
#include <list>
#include <OBB.h>
#include <CollisionUtils.h>
#include <memory>
#include <Collider.h>

namespace MyGame {
    class CollisionManager {
    private: // シングルトンインスタンス
        static std::unique_ptr<CollisionManager> instance_;
        // コピーコンストラクタを防ぐ
        CollisionManager(const CollisionManager&) = delete;
        CollisionManager& operator=(const CollisionManager&) = delete;
    public: // メンバ関数 
        CollisionManager() = default;
        // シングルトンインスタンスの取得
        static CollisionManager* GetInstance();

        // 全判定の実行
        void CheckAllCollisions();
		// コライダーの追加
        void AddCollider(Collider* collider) { colliders_.push_back(collider); }
		// コライダーの削除
        void RemoveCollider(Collider* collider) { colliders_.remove(collider); }

    private:
        std::list<Collider*> colliders_;
    };
}