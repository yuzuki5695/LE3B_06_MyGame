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

    void BulletManager::Initialize() {
#ifdef USE_IMGUI
        EditorTypes::EditorObjectInfo info;
        info.name = "Bullet Manager";                                 // エディタメニューに表示される名前
        info.category = EditorTypes::ObjectCategory::List;            // リスト/マネジメントカテゴリ
        info.objectPtr = this;
        info.drawEditor = [this]() {
            ImGui::SeparatorText("Player Bullet");
            // ImGuiでサイズを変更（ドラッグで調整可能に）
            if (ImGui::DragFloat3("Player Bullet Size", &commonPlayerBulletSize_.x, 0.05f, 0.1f, 10.0f)) {
                // 値が変わったら、現在画面内に存在するすべての弾に即座に反映
                for (auto& bullet_ : bullets_) {
                    if (bullet_ && bullet_->IsActive()) {
                        if (PlayerBullet* Player = dynamic_cast<PlayerBullet*>(bullet_.get())) {
                            Player->SetColliderSize(commonPlayerBulletSize_);
                        }
                    }
                }
            }

            ImGui::SeparatorText("Enemy Bullet Settings");
            if (ImGui::DragFloat3("Enemy Bullet Size", &commonEnemyBulletSize_.x, 0.05f, 0.1f, 10.0f)) {
                // 値が変わったら、現在画面内に存在するすべての弾に即座に反映
                for (auto& bullet_ : bullets_) {
                    if (bullet_ && bullet_->IsActive()) {
                        if (EnemyBullet* Enemy = dynamic_cast<EnemyBullet*>(bullet_.get())) {
                            Enemy->SetColliderSize(commonEnemyBulletSize_);
                        }
                    }
                }
            }

            };
        // エディタシステムに登録
        EditorEntityRegistry::Instance().Register(info);
#endif // USE_IMGUI
    }

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

    void BulletManager::Draw() {
        for (std::unique_ptr<BaseBullet>& bullet_ : bullets_) {
            bullet_->Draw();
        }
    }

    void BulletManager::SpawnPlayerBullet(const Transform& transform, const Vector3& velocity) {
        std::unique_ptr<BaseBullet> bullet_ = std::make_unique<PlayerBullet>();
        bullet_->Initialize(transform, velocity);
        // 最新の共通サイズを適用
        if (PlayerBullet* Player = dynamic_cast<PlayerBullet*>(bullet_.get())) {
            Player->SetColliderSize(commonPlayerBulletSize_);
        }
        bullets_.push_back(std::move(bullet_));
    }

    void BulletManager::SpawnEnemyBullet(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) {
        std::unique_ptr<BaseBullet> bullet_ = std::make_unique<EnemyBullet>();
        bullet_->Initialize(transform, velocity);
        // 最新の共通サイズを適用
        if (EnemyBullet* Enemy = dynamic_cast<EnemyBullet*>(bullet_.get())) {
            Enemy->SetColliderSize(commonEnemyBulletSize_);
        }
        bullets_.push_back(std::move(bullet_));
    }
}