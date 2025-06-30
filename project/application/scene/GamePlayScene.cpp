#include "GamePlayScene.h"
#include<TextureManager.h>
#include<ModelManager.h>
#include<SpriteCommon.h>
#include<Object3dCommon.h>
#include<Input.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include<SceneManager.h>
#include <ParticleCommon.h>
#include <ParticleManager.h>
#include <numbers>
#include <CameraManager.h>
#include<CharacterManager.h>

void GamePlayScene::Finalize() {
	CharacterManager::GetInstance()->Clear(); // キャラクターの解放処理
	BulletManager::GetInstance()->Finalize(); // 弾の解放処理
}

void GamePlayScene::Initialize() {

    // カメラの初期化
    CameraManager::GetInstance()->Initialize();

    // テクスチャを読み込む
    TextureManager::GetInstance()->LoadTexture("Resources/uvChecker.png");
    TextureManager::GetInstance()->LoadTexture("Resources/monsterBall.png");

    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel("plane.obj");
    ModelManager::GetInstance()->LoadModel("terrain.obj");

    // 音声ファイルを追加
    soundData = SoundLoader::GetInstance()->SoundLoadWave("Resources/Alarm01.wav");

    // 音声プレイフラグ
    soundfige = 0;

    grass = Object3d::Create("terrain.obj", Transform({ {1.0f, 1.0f, 100.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 70.0f} }));
                                            
    // パーティクルグループ生成
    ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Resources/uvChecker.png", "plane.obj", VertexType::Model);            // モデルで生成

    // キャラクターの生成
    CharacterManager::GetInstance()->Clear(); // リストクリア
    auto player = std::make_unique<Player>();
    Player* playerPtr = player.get();
    CharacterManager::GetInstance()->SetPlayer(std::move(player));


    for (int i = 0; i < 5; ++i) {
        std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(); 
        enemy->SetPlayer(playerPtr);   // プレイヤーのポインタをセット！
        CharacterManager::GetInstance()->AddEnemy(std::move(enemy));
    }
    

    CharacterManager::GetInstance()->Initialize(); // 登録クラスを全て初期化

    // Bulletマネージャの初期化
    BulletManager::GetInstance()->Initialize();
}

void GamePlayScene::Update() {
#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI
    // object3d

    // Camera
    CameraManager::GetInstance()->DrawImGui();

#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了 
    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理

    // 更新処理
    grass->Update();


    if (CameraManager::GetInstance()->Getmovefige()) {
        CheckBulletEnemyCollisions();  // 当たり判定(プレイヤーの球と敵)
        CheckPlayerEnemyCollisions();
        CheckEnemyBulletPlayerCollisions();
    }

	// キャラクターの更新処理
    CharacterManager::GetInstance()->Update();
    // Bulletマネージャの更新処理
    BulletManager::GetInstance()->Update();

    ParticleManager::GetInstance()->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理


#pragma endregion 全てのSprite個々の更新処理
#ifdef USE_IMGUI
    // ImGuiの描画前準備
    ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI
}

void GamePlayScene::Draw() {

#pragma region 全てのObject3d個々の描画処理
    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();

    grass->Draw();

    // キャラクターの描画処理
    CharacterManager::GetInstance()->Draw();
    // Bulletマネージャの描画処理
    BulletManager::GetInstance()->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
    ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();

#pragma endregion 全てのSprite個々の描画処理
}

void GamePlayScene::CheckBulletEnemyCollisions() {
    const auto& bullets = BulletManager::GetInstance()->GetPlayerBullets();
    const auto& enemies = CharacterManager::GetInstance()->GetEnemies();

    for (const std::unique_ptr<PlayerBullet>& bullet : bullets) {
        if (!bullet->IsActive()) continue;

        for (Enemy* enemy : enemies) {
            if (!enemy->IsActive()) continue;

            Vector3 delta = bullet->GetPosition() - enemy->GetPosition();
            Vector3 collisionDist = bullet->GetRadius() + enemy->GetRadius();

            if (std::abs(delta.x) <= collisionDist.x &&
                std::abs(delta.y) <= collisionDist.y &&
                std::abs(delta.z) <= collisionDist.z) {

                bullet->SetInactive();
                enemy->SetInactive();

                // パーティクルなど
                break;
            }
        }
    }
}

void GamePlayScene::CheckPlayerEnemyCollisions() {
    Player* player = CharacterManager::GetInstance()->GetPlayer();
    const auto& enemies = CharacterManager::GetInstance()->GetEnemies();

    // プレイヤーがいない or 無効化されていたら何もしない
    if (!player || !player->IsActive()) {
        return;
    }

    const Vector3& playerPos = player->GetPosition();
    float playerRadius = player->GetRadius().x; // 半径 = スケールの半分（仮）

    for (Enemy* enemy : enemies) {
        if (!enemy->IsActive()) continue;

        const Vector3& enemyPos = enemy->GetPosition();
        float enemyRadius = enemy->GetRadius().x;

        Vector3 delta = playerPos - enemyPos;
        float dist = Length(delta);
        float collisionDist = playerRadius + enemyRadius;

        if (dist <= collisionDist) {
            // 両者を無効化
            player->SetInactive();
            enemy->SetInactive();

            break; // 一体でも当たったら処理終了
        }
    }
}

void GamePlayScene::CheckEnemyBulletPlayerCollisions() {
    const auto& bullets = BulletManager::GetInstance()->GetEnemyBullets();
    Player* player = CharacterManager::GetInstance()->GetPlayer();

    if (!player || !player->IsActive()) return;

    const Vector3& playerPos = player->GetPosition();
    const Vector3& playerRadius = player->GetRadius();

    for (const std::unique_ptr<EnemyBullet>& bullet : bullets) {
        if (!bullet->IsActive()) continue;

        Vector3 delta = bullet->GetPosition() - playerPos;
        Vector3 collisionDist = bullet->GetRadius() + playerRadius;

        if (std::abs(delta.x) <= collisionDist.x &&
            std::abs(delta.y) <= collisionDist.y &&
            std::abs(delta.z) <= collisionDist.z) {
            
            bullet->SetInactive();
            player->SetInactive();  // プレイヤーを無効にする

            // パーティクルやエフェクトの追加処理
            break;  // 一発当たったら終了
        }
    }
}