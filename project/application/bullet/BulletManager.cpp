#include "BulletManager.h"

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
    // 各弾データをクリア
    playerBullets_.clear();
    enemyBullets_.clear();
}

///====================================================
/// 更新処理
///====================================================
void BulletManager::Update() {
    // プレイヤー弾の更新
    for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_) {
        if (bullet->IsActive()) {
            bullet->Update();
        }
    }

    // 敵弾の更新
    for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
        if (bullet->IsActive()) {
            bullet->Update();
        }
    }

    // 無効化された弾を削除
    std::erase_if(playerBullets_, [](const std::unique_ptr<PlayerBullet>& b) {
        return !b->IsActive();
        });
    std::erase_if(enemyBullets_, [](const std::unique_ptr<EnemyBullet>& b) {
        return !b->IsActive();
        });
}

///====================================================
/// 描画処理
///====================================================
void BulletManager::Draw() {
    // --- 無効化された弾を削除 --- //
    for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_) {
        if (bullet->IsActive()) {
            bullet->Draw();
        }
    }

    for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
        if (bullet->IsActive()) {
            bullet->Draw();
        }
    }
}

///====================================================
/// プレイヤー弾の追加登録
///====================================================
void BulletManager::AddPlayerBullet(std::unique_ptr<PlayerBullet> bullet) {
    playerBullets_.push_back(std::move(bullet));
}

///====================================================
/// 敵弾の追加登録
///====================================================
void BulletManager::AddEnemyBullet(std::unique_ptr<EnemyBullet> bullet) {
    enemyBullets_.push_back(std::move(bullet));
}

///====================================================
/// プレイヤー弾の参照を返す
///====================================================
const std::vector<std::unique_ptr<PlayerBullet>>& BulletManager::GetPlayerBullets() const {
    return playerBullets_;
}

///====================================================
/// 敵弾の参照を返す
///====================================================
const std::vector<std::unique_ptr<EnemyBullet>>& BulletManager::GetEnemyBullets() const {
    return enemyBullets_;
}