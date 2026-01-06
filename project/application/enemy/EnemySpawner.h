#pragma once
#include <Player.h>
#include <CameraManager.h>
#include <Enemy.h>


class EnemySpawner {
public:
    void Initialize(Player* player, CameraManager* camera, std::vector<std::unique_ptr<Enemy>>* enemies);

    void Update();

private:
    void EnemySpawn();
    void SpawnVFormation(const EnemySpawnTrigger& trigger);
    void SpawnReverseStepFormation(const EnemySpawnTrigger& trigger);
    void SpawnZigZagFormation(const EnemySpawnTrigger& trigger);

private:
    Player* player_ = nullptr;
    CameraManager* cameraManager_ = nullptr;
    std::vector<std::unique_ptr<Enemy>>* enemies_ = nullptr;
    std::vector<EnemySpawnTrigger> spawnTriggers_;
};
