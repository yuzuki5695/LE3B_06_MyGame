#pragma once
#include <vector>
#include <list>
#include <OBB.h>
#include <CollisionUtils.h>
#include <memory>
#include <Collider.h>

namespace MyGame {
    /// <summary>
	/// 衝突管理クラス
    /// </summary>
    class CollisionManager {
    private: // シングルトンインスタンス
        static std::unique_ptr<CollisionManager> instance;
        // コピーコンストラクタを防ぐ
        CollisionManager(const CollisionManager&) = delete;
        CollisionManager& operator=(const CollisionManager&) = delete;
    public: // メンバ関数 
        CollisionManager() = default;
        // シングルトンインスタンスの取得
        static CollisionManager* GetInstance();             
        // 終了
        void Finalize();

        // 全判定の実行
        void CheckAllCollisions();

        void RegisterCollider(Collider* collider) {
            colliders_.push_back(collider);
        }

        void UnregisterCollider(Collider* collider) {
            colliders_.remove(collider);
        }
    private: // メンバ変数
		std::list<Collider*> colliders_; // 登録されたコライダーのリスト
    };
}