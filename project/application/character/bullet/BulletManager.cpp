#include "BulletManager.h"
#include <PlayerBullet.h>

using namespace MyEngine;

namespace MyGame {
    // シングルトン用インスタンス
    BulletManager* BulletManager::instance = nullptr;

    ///====================================================
    /// シングルトンインスタンスの取得
    ///====================================================
    BulletManager* BulletManager::GetInstance() {
        // まだインスタンスが生成されていなければ作成
        if (instance == nullptr) {
            instance = new BulletManager;
        }
        return instance;
    }

    ///====================================================
    /// 終了処理
    ///====================================================
    void BulletManager::Finalize() {
        // インスタンスを削除してnullptrに戻す
        delete instance;
        instance = nullptr;
    }

    void BulletManager::Update() {
        // 全弾更新
        for (auto& bullet : bullets_) {
            bullet->Update();
        }

        // 非アクティブ削除（超重要）
        bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<BaseBullet>& bullet)
            {return !bullet->IsActive(); }),
            bullets_.end()
        );
    }

    void BulletManager::Draw() {
        for (auto& bullet : bullets_) {
            bullet->Draw();
        }
    }

    void BulletManager::SpawnPlayerBullet(const Transform& transform, const Vector3& velocity) {
        auto bullet = std::make_unique<PlayerBullet>();
        bullet->Initialize(transform, velocity);
        bullets_.push_back(std::move(bullet));
    }
}