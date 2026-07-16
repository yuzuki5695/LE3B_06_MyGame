#include "BulletManager.h"
#include <PlayerBullet.h>
#include <EnemyBullet.h>
#include <EditorEntityRegistry.h>
#include <EditorTypes.h>
#include <LineRenderer.h>
#include <ImGuiManager.h>

using namespace MyEngine;

namespace MyGame {
    // シングルトン用インスタンス
    BulletManager* BulletManager::instance_ = nullptr;

    ///====================================================
    /// シングルトンインスタンスの取得
    ///====================================================
    BulletManager* BulletManager::GetInstance() {
        // まだインスタンスが生成されていなければ作成
        if (instance_ == nullptr) {
            instance_ = new BulletManager;
        }
        return instance_;
    }

    ///====================================================
    /// 終了処理
    ///====================================================
    void BulletManager::Finalize() {
        // インスタンスを削除してnullptrに戻す
        delete instance_;
        instance_ = nullptr;
    }
    ///====================================================
    /// 初期化処理
    ///====================================================
    void BulletManager::Initialize() {}
    ///====================================================
    /// 更新処理
    ///====================================================
    void BulletManager::Update() {
        // 全弾更新
        for (std::unique_ptr<BaseBullet>& bullet_ : bullets_) {
            bullet_->Update();
        }
        // 非アクティブ削除
        bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
            [](const auto& bullet_) {
                return !bullet_->IsActive();
            }),
            bullets_.end()
        );
    }
    ///====================================================
    /// 描画処理
    ///====================================================
    void BulletManager::Draw() {
        for (std::unique_ptr<BaseBullet>& bullet_ : bullets_) {
            bullet_->Draw();
        }
    }
    ///====================================================
    /// プレイヤー弾生成  
    ///====================================================
    void BulletManager::SpawnPlayerBullet(const Transform& transform, const Vector3& velocity) {
        std::unique_ptr<BaseBullet> bullet_ = std::make_unique<PlayerBullet>();
        bullet_->Initialize(transform, velocity);
        // 最新の共通サイズを適用
        if (PlayerBullet* Player = dynamic_cast<PlayerBullet*>(bullet_.get())) {
            Player->SetColliderSize(commonplayerbulletsize_);
        }
        bullets_.push_back(std::move(bullet_));
    }
    ///====================================================
    /// 敵弾生成
    ///====================================================
    void BulletManager::SpawnEnemyBullet(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) {
        std::unique_ptr<BaseBullet> bullet_ = std::make_unique<EnemyBullet>();
        bullet_->Initialize(transform, velocity);
        // 最新の共通サイズを適用
        if (EnemyBullet* Enemy = dynamic_cast<EnemyBullet*>(bullet_.get())) {
            Enemy->SetColliderSize(commonenemybulletsize_);
        }
        bullets_.push_back(std::move(bullet_));
    }
}