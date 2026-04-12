#pragma once
#include <Player.h>
#include <CameraManager.h>
#include <Enemy.h>
#include <random>

//
///// <summary>
///// 敵の出現条件（トリガー）を定義する構造体
///// </summary>
//struct EnemySpawnTrigger {
//    Vector3 Position;   // 出現位置
//    int spawnCount;     // 同時に出現させる数
//    bool hasSpawned;    // すでに出現済みかどうか
//    MoveType moveType;  // 敵の移動タイプ
//};

namespace MyGame {
    class EnemySpawner {
    public:
           void Update();
           
           void SpawnEnemies(int count);

           void SetEnemies(std::vector<std::unique_ptr<Enemy>>* enemies) {
               enemies_ = enemies;
           }
           void SetPlayer(Player* player) { player_ = player; }
    private:
        float spawnInterval_ = 3.0f;   // 何秒ごと
        float spawnTimer_ = 0.0f;
        int spawnCount_ = 3;           // 1回で出す数
        std::vector<std::unique_ptr<Enemy>>* enemies_ = nullptr;
            
        std::mt19937 randomEngine;               // 乱数生成器

        Player* player_ = nullptr;
        //CameraManager* cameraManager_ = nullptr;

        //std::vector<EnemySpawnTrigger> spawnTriggers_;


        //// =========================
        //// ランダム出現用
        //// =========================
        //float randomSpawnTimer_ = 0.0f;           // 経過時間
        //float nextRandomSpawnTime_ = 2.0f;        // 次に出現させるまでの時間（秒）
        //std::mt19937 randomEngine;               // 乱数生成器
    };
}