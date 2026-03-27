#pragma once
#include <vector>
#include <list>
#include <OBB.h>
#include <CollisionUtils.h>
#include <memory>

// コライダーの基底クラス
class Collider {
public:
    virtual ~Collider() = default;
    virtual void OnCollision(Collider* other) = 0; // 衝突時に呼ばれるコールバック
    virtual OBB GetOBB() const = 0;

    // 衝突属性の設定・取得

    // 自分がの種別番号
    void SetCollisionAttribute(uint32_t attribute) { collisionAttribute_ = attribute; }
    uint32_t GetCollisionAttribute() const { return collisionAttribute_; }
    // 相手の種別番号
    void SetCollisionMask(uint32_t mask) { collisionMask_ = mask; }
    uint32_t GetCollisionMask() const { return collisionMask_; }
protected:
    uint32_t collisionAttribute_ = 0xffffffff; // 自分の種別
    uint32_t collisionMask_ = 0xffffffff;      // 当たりたい相手の種別
};

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


    // コライダーの登録・解除
    void RegisterCollider(Collider* collider) { colliders_.push_back(collider); }
    void UnregisterCollider(Collider* collider) { colliders_.remove(collider); }

    // 全判定の実行
    void CheckAllCollisions();

private:
    std::list<Collider*> colliders_;
};