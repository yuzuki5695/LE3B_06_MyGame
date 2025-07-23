#pragma once
#include "BaseBullet.h"
#include <vector>
#include <memory>
#include<PlayerBullet.h>
//#include<EnemyBullet.h>

class BulletManager {
private:
    static BulletManager* instance;
    BulletManager(BulletManager&) = delete;
    BulletManager& operator=(BulletManager&) = delete;
public: // メンバ関数 
    BulletManager() = default;
    ~BulletManager() = default;
    // シングルトンインスタンスの取得
    static BulletManager* GetInstance();
    // 初期化・終了
    void Initialize();
    void Finalize();

    // 更新・描画
    void Update();
    void Draw();
        
    void AddPlayerBullet(std::unique_ptr<PlayerBullet> bullet);
  ///  void AddEnemyBullet(std::unique_ptr<EnemyBullet> bullet);

    const std::vector<std::unique_ptr<PlayerBullet>>& GetPlayerBullets() const;
   // const std::vector<std::unique_ptr<EnemyBullet>>& GetEnemyBullets() const;

private:
    std::vector<std::unique_ptr<PlayerBullet>> playerBullets_;
 //   std::vector<std::unique_ptr<EnemyBullet>> enemyBullets_;
};