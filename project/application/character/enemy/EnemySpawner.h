#pragma once
#include <Player.h>
#include <CameraManager.h>
#include <Enemy.h>
#include <random>

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

    };
}