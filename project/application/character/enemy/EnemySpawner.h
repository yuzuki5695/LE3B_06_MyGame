#pragma once
#include <Player.h>
#include <CameraManager.h>
#include <Enemy.h>


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
     //   void Initialize(Player* player, CameraManager* camera, std::vector<std::unique_ptr<Enemy>>* enemies);

       // void Update();

    private:
        //void EnemySpawn();
    private:
        //Player* player_ = nullptr;
        //CameraManager* cameraManager_ = nullptr;
        //std::vector<std::unique_ptr<Enemy>>* enemies_ = nullptr;
        //std::vector<EnemySpawnTrigger> spawnTriggers_;


        //// =========================
        //// ランダム出現用
        //// =========================
        //float randomSpawnTimer_ = 0.0f;           // 経過時間
        //float nextRandomSpawnTime_ = 2.0f;        // 次に出現させるまでの時間（秒）
        //std::mt19937 randomEngine;               // 乱数生成器
    };
}