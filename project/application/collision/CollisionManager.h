//#pragma once
//#include <vector>
//#include <memory>
//
//// 前方宣言
//class Player;
//class Enemy;
//class PlayerBullet;
//class EnemyBullet;
//
//class CollisionManager {
//public:
//
//    void SetPlayer(Player* player);
//
//    void SetEnemies(std::vector<std::unique_ptr<Enemy>>* enemies);
//
//    void CheckAllCollisions();
//
//private:
//
//    void CheckBulletEnemy();
//    void CheckEnemyBulletPlayer();
//    void CheckEnemyPlayer();
//
//private:
//
//    Player* player_ = nullptr;
//
//    std::vector<std::unique_ptr<Enemy>>* enemies_ = nullptr;
//};