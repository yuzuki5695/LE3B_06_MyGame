#pragma once
#include "BaseBullet.h"
#include <vector>
#include <memory>
#include<PlayerBullet.h>
#include<EnemyBullet.h>

/// <summary>
/// 各弾の更新・描画を管理するクラス
/// 各シーンで発射された弾を追加,保持する
/// 弾の生成は AddPlayerBullet / AddEnemyBullet 経由で行う
/// シングルトンとして全体からアクセス可能
/// </summary>
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
    /// <summary>
    /// 終了処理 
    /// </summary> 
    void Finalize();
    /// <summary>
    /// 更新処理 
    /// </summary>
    void Update();
    /// <summary>
    /// 描画処理 
    /// </summary>
    void Draw();
    /// <summary>
    /// プレイヤーの弾を登録
    /// 外部から生成された弾をBulletManagerに登録する。
    /// 登録された弾はUpdateやDraw処理で管理されるようになる。
    /// </summary>
    void AddPlayerBullet(std::unique_ptr<PlayerBullet> bullet);
    /// <summary>
    /// 敵の弾を登録
    /// 外部から生成された弾をBulletManagerに登録する。
    /// 登録された弾はUpdateやDraw処理で管理されるようになる。
    /// </summary>
    void AddEnemyBullet(std::unique_ptr<EnemyBullet> bullet);
private:
    // 弾のリスト
    std::vector<std::unique_ptr<PlayerBullet>> playerBullets_; // プレイヤー
    std::vector<std::unique_ptr<EnemyBullet>> enemyBullets_;   // 敵
public: // アクセッサ（Getter / Setter）   
    // 弾のリストを外部から読み取る
    const std::vector<std::unique_ptr<PlayerBullet>>& GetPlayerBullets() const; // プレイヤー
    const std::vector<std::unique_ptr<EnemyBullet>>& GetEnemyBullets() const;   // 敵
};