#pragma once
#include "BaseBullet.h"
#include <vector>
#include <memory>
#include<PlayerBullet.h>
#include<EnemyBullet.h>

/// 各弾の更新・描画を管理するクラス
/// 各シーンで発射された弾を追加,保持する
/// 弾の生成は AddPlayerBullet / AddEnemyBullet 経由で行う
/// シングルトンとして全体からアクセス可能
class BulletManager {
private: // シングルトンインスタンス
    static BulletManager* instance;
    // コピーコンストラクタを防ぐ
    BulletManager(BulletManager&) = delete;
    BulletManager& operator=(BulletManager&) = delete;
public: // メンバ関数 
    BulletManager() = default;
    ~BulletManager() = default;
    // シングルトンインスタンスの取得
    static BulletManager* GetInstance();
	// 終了処理
    void Finalize();
    // 更新処理
    void Update();
	// 描画処理 
    void Draw();

	// 弾の追加
    void AddPlayerBullet(std::unique_ptr<PlayerBullet> bullet); // プレイヤーの弾を受け取る
	void AddEnemyBullet(std::unique_ptr<EnemyBullet> bullet);   // 敵の弾を受け取る
private:
	// 弾のリスト
	std::vector<std::unique_ptr<PlayerBullet>> playerBullets_; // プレイヤー
	std::vector<std::unique_ptr<EnemyBullet>> enemyBullets_;   // 敵
public: // ゲッター    
    // 弾のリストを外部から読み取る
    const std::vector<std::unique_ptr<PlayerBullet>>& GetPlayerBullets() const; // プレイヤー
	const std::vector<std::unique_ptr<EnemyBullet>>& GetEnemyBullets() const;   // 敵
};