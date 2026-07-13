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
        static std::unique_ptr<CollisionManager> instance_;
        // コピーコンストラクタを防ぐ
        CollisionManager(const CollisionManager&) = delete;
        CollisionManager& operator=(const CollisionManager&) = delete;
    public: // メンバ関数 
        CollisionManager() = default;
        // シングルトンインスタンスの取得
        static CollisionManager* GetInstance();             
        /// <summary>
		/// 終了処理
        /// </summary>
        void Finalize();
        /// <summary>
		/// 全てのコライダーの組み合わせに対して衝突判定を行い、衝突があればコールバックを呼び出す。
        /// </summary>
        void CheckAllCollisions();
        /// <summary>
		/// コライダーを登録する。登録されたコライダーは衝突判定の対象になる。
        /// </summary>
        /// <param name="collider"></param>
        void RegisterCollider(Collider* collider) { colliders_.push_back(collider); }
        /// <summary>
		/// コライダーの登録を解除する。登録解除されたコライダーは衝突判定の対象から外れる。
        /// </summary>
        /// <param name="collider"></param>
        void UnregisterCollider(Collider* collider) { colliders_.remove(collider); }
    private: // メンバ変数
		std::list<Collider*> colliders_; // 登録されたコライダーのリスト
    };
}