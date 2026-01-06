#pragma once
#include <Player.h>
#include <CameraManager.h>
#include <Enemy.h>
#include<EnemyLoader.h>

/// <summary>
/// 敵の出現条件（トリガー）を定義する構造体
/// </summary>
struct EnemySpawnTrigger {
    Vector3 Position;   // 出現位置
    int spawnCount;     // 同時に出現させる数
    bool hasSpawned;    // すでに出現済みかどうか
    MoveType moveType;  // 敵の移動タイプ
};

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

    // キャラクターローダー
    std::unique_ptr<EnemyLoader> levelLoader_ = nullptr;
    // レベルデータ格納用インスタンスを生成
    LevelEnemyData* levelData = nullptr;
};