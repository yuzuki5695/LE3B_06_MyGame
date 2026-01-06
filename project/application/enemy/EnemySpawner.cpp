#include"EnemySpawner.h"
#include <MatrixVector.h>

using namespace MatrixVector;

void EnemySpawner::Initialize(Player* player, CameraManager* cameraManager, std::vector<std::unique_ptr<Enemy>>* enemies) {
    player_ = player;
    cameraManager_ = cameraManager;
    enemies_ = enemies;

    //spawnTriggers_ = {
    //    {Vector3{0.0f,0.0f,150.0f}, 5, false, MoveType::Horizontal},
    //    {Vector3{0.0f,0.0f,250.0f}, 5, false, MoveType::Vertical},
    //    {Vector3{0.0f,0.0f,350.0f}, 5, false, MoveType::None},
    //};
       
    // Loader を生成
    levelLoader_ = std::make_unique<EnemyLoader>();

    // レベルデータ読み込み
    levelData = levelLoader_->LoadFile("Enemy");


    for (const auto& e : levelData->enemies) {
        EnemySpawnTrigger trigger;
        trigger.Position = e.translation;
        trigger.spawnCount = 5;
        trigger.hasSpawned = false;

        trigger.moveType =
            (e.enemyType == 1) ? MoveType::Horizontal :
            (e.enemyType == 2) ? MoveType::Vertical :
            MoveType::None;

        spawnTriggers_.push_back(trigger);
    }
}

void EnemySpawner::Update() {
    EnemySpawn();
}
void EnemySpawner::EnemySpawn() {
    Vector3 playerPos = player_->GetPosition();
    GamePlayCamera* gameCam = cameraManager_->GetGameplayCamera();
    if (!gameCam) return;

    Vector3 forward = gameCam->GetForward();
    Vector3 right = Normalize(Cross({0,1,0}, forward));
    Vector3 up = Normalize(Cross(forward, right));

    for (size_t i = 0; i < spawnTriggers_.size(); ++i) {
        auto& trigger = spawnTriggers_[i];

        float distanceForward =
            Dot((trigger.Position - playerPos), forward);

        if (!trigger.hasSpawned &&
            distanceForward >= 0.0f &&
            distanceForward < 10.0f) {

            switch (i) {
            case 0: SpawnVFormation(trigger); break;
            case 1: SpawnVFormation(trigger); break;
            case 2: SpawnVFormation(trigger); break;
            }

            trigger.hasSpawned = true;
        }
    }
}

///====================================================
/// 敵出現パターン：V字フォーメーション（姿勢対応）
///====================================================
void EnemySpawner::SpawnVFormation(const EnemySpawnTrigger& trigger) {
    // プレイヤー・カメラ情報を取得
    Vector3 playerPos = player_->GetPosition();
    GamePlayCamera* gameCam = CameraManager::GetInstance()->GetGameplayCamera(); 
    if (!gameCam) return;

    Vector3 forward = Normalize(gameCam->GetForward());
    Vector3 right   = Normalize(Cross({ 0,1,0 }, forward));
    Vector3 up      = Normalize(Cross(forward, right));

    // 敵配置設定
    int activated = 0;

    const float forwardDist = 60.0f;  // 前方距離
    const float baseY = 1.0f;
    const float stepX = 3.0f;
    const float stepY = -2.0f;

    for (auto& enemy : *enemies_) {
        if (!enemy->IsActive()) {
            float x = 0.0f;
            float y = 0.0f;

            switch (activated) {
            case 0: x =  0.0f; y =  0.0f; break;               // 中央
            case 1: x = -stepX; y = stepY; break;              // 左上
            case 2: x =  stepX; y = stepY; break;              // 右上
            case 3: x = -stepX * 2; y = stepY * 2; break;      // 左下
            case 4: x =  stepX * 2; y = stepY * 2; break;      // 右下
            default:
                x = (activated - 2) * stepX;
                y = stepY * 3;
                break;
            }

            Vector3 spawnPos =
                playerPos +
                forward * forwardDist +
                right * x +
                up * (baseY + y);

           
//            enemy->SetnewTranslate(spawnPos, trigger.moveType);
            enemy->Spawn(spawnPos, trigger.moveType);
            enemy->SetActive(true);

            ++activated;
            if (activated >= trigger.spawnCount) break;
        }
    }
}



///====================================================
/// 敵出現パターン：ジグザグフォーメーション
///====================================================
void EnemySpawner::SpawnZigZagFormation(const EnemySpawnTrigger& trigger) {

}

///====================================================
/// 敵出現パターン：逆ステップフォーメーション（姿勢対応）
///====================================================
void EnemySpawner::SpawnReverseStepFormation(const EnemySpawnTrigger& trigger) {

}