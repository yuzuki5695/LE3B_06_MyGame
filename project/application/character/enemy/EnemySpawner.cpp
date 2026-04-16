#include "EnemySpawner.h"
#include <CameraManager.h>
#include <EnemyState.h>
#include <MatrixVector.h>

using namespace MyEngine;
using namespace MatrixVector;

namespace MyGame {

    void EnemySpawner::Update() {
        spawnTimer_ += 1.0f / 60.0f;

        if (spawnTimer_ >= spawnInterval_) {
            SpawnEnemies(spawnCount_);
            spawnTimer_ = 0.0f;
        }
    }

    void EnemySpawner::SpawnEnemies(int count) {
        //GamePlayCamera* cam = CameraManager::GetInstance()->GetGameplayCamera();
        //if (!cam || !player_) return;

        //Vector3 playerPos = player_->GetObject3d()->GetTranslate();

        //Vector3 forward = Normalize(cam->GetForward());
        //Vector3 right = Normalize(Cross({ 0,1,0 }, forward));
        //Vector3 up = Normalize(Cross(forward, right));

        //// 乱数エンジンを初期化
        //std::random_device rd;// 乱数生成器
        //randomEngine = std::mt19937(rd());

        //std::uniform_real_distribution<float> distX(-15.0f, 15.0f);
        //std::uniform_real_distribution<float> disty(-4.0f, 8.0f);
        //std::uniform_real_distribution<float> distz(80.0f, 250.0f);

        //int spawned = 0;

        //for (auto& enemy : *enemies_) {
        //    if (!enemy->IsActive() && spawned < count) {

        //        // ランダム値生成
        //        float randX = distX(randomEngine);
        //        float randY = disty(randomEngine);
        //        float randZ = distz(randomEngine);

        //        Vector3 spawnPos = playerPos + forward * randZ + right * randX + up * randY;
        //        enemy->SetActive(true);
        //        enemy->GetObject3d()->SetTranslate(spawnPos);
        //        enemy->ChangeState(std::make_unique<EnemyIdle>());
        //        spawned++;
        //    }
        //}
    }
}