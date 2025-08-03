#include "GamePlayScene.h"
#include<SceneManager.h>
#include<TextureManager.h>
#include<ModelManager.h>
#include<SpriteCommon.h>
#include<Object3dCommon.h>
#include <CameraManager.h>
#include <ParticleCommon.h>
#include<Input.h>
#include<Controller.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include<SkyboxCommon.h>
#include<Player.h>
#include <BulletManager.h>
#include<MatrixVector.h>

using namespace MatrixVector;

void GamePlayScene::Finalize() {
	BulletManager::GetInstance()->Finalize(); // 弾の解放処理
}

void GamePlayScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));

    // テクスチャを読み込む
    TextureManager::GetInstance()->LoadTexture("uvChecker.png");
    TextureManager::GetInstance()->LoadTexture("monsterBall.png");
    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel("terrain.obj");
    ModelManager::GetInstance()->LoadModel("monsterBallUV.obj");
    ModelManager::GetInstance()->LoadModel("Bullet/PlayerBullet.obj");
    ModelManager::GetInstance()->LoadModel("Tile.obj");

    // オブジェクトの作成
    transform_ = { {15.0f, 1.0f, 100.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, -8.0f, 50.0f} };

    grass = Object3d::Create("Tile.obj", transform_);

    
    player_ = std::make_unique<Player>();
	player_->Initialize(); // プレイヤーの初期化
        
    
    // 敵出現トリガー
    spawnTriggers_ = {
        {40.0f, 3, false}, // 30m通過時に3体出現
        {90.0f, 3, false}, // 60m通過時に3体出現
    };

    // 敵をリストに追加して初期化
    for (int i = 0; i < MAX_ENEMY; ++i) {
        std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
        enemy->Initialize();
        enemy->SetPlayer(player_.get());
        enemy->SetActive(false);  // 非アクティブにしておく
        enemies_.emplace_back(std::move(enemy));
    }

    // Bulletマネージャの初期化
    BulletManager::GetInstance()->Initialize();
}

void GamePlayScene::Update() {
    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();

    // ENTERキーを押したら
    if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
        // シーン切り替え
        //SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
    }

#pragma region 全てのObject3d個々の更新処理

    if (player_->GetPosition().z >= 180.0f) {
        end = true;

    }

    if (!end) {

        // 敵出現動作
        EnemySpawn();

        CheckBulletEnemyCollisionsOBB();

        // 更新処理
        grass->Update();

        player_->Update();

        // 敵の更新
        for (auto& enemy : enemies_) {
            if (enemy->IsActive()) {
                enemy->Update();
            }
        }

        // Bulletマネージャの更新処理
        BulletManager::GetInstance()->Update();
    }


    // 死んだ敵の削除
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& e) {
                return e->IsDead();  // ← 出現前の非アクティブは残す！
            }),
        enemies_.end());

    ParticleManager::GetInstance()->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理


#pragma endregion 全てのSprite個々の更新処理

#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI
    Object3dCommon::GetInstance()->DrawImGui();

    // Camera
    CameraManager::GetInstance()->DrawImGui();

#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了 
}

void GamePlayScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();

    // 描画処理

    grass->Draw();

    player_->Draw();

    // 敵の更新
    for (auto& enemy : enemies_) {
        if (enemy->IsActive()) {
            enemy->Draw();
        }
    }

    // Bulletマネージャの描画処理
    BulletManager::GetInstance()->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
    ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();

    //   player_->DrawSprite();

#pragma endregion 全てのSprite個々の描画処理
}

void GamePlayScene::EnemySpawn() {
    // プレイヤーの位置取得
    float playerZ = player_->GetPosition().z;

    // 敵出現トリガー処理
    for (auto& trigger : spawnTriggers_) {
        if (!trigger.hasSpawned && playerZ >= trigger.zThreshold) {
            int activated = 0;
            for (auto& enemy : enemies_) {
                if (!enemy->IsActive()) {
                    enemy->SetInitialize(trigger.zThreshold);
                    enemy->SetActive(true); // ←トリガーのZ位置を渡す！
                    ++activated;
                    if (activated >= trigger.spawnCount) break;
                }
            }
            trigger.hasSpawned = true;
        }
    }
}


bool GamePlayScene::IsOBBIntersect(const OBB& a, const OBB& b) {
    const float EPSILON = 1e-5f;
    Vector3 T = b.center - a.center;

    // Aのローカル軸
    Vector3 A[3] = { a.axis[0], a.axis[1], a.axis[2] };
    // Bのローカル軸
    Vector3 B[3] = { b.axis[0], b.axis[1], b.axis[2] };

    float R[3][3];
    float AbsR[3][3];

    // 回転行列R = A軸とB軸の内積
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            R[i][j] = Dot(A[i], B[j]);
            AbsR[i][j] = std::abs(R[i][j]) + EPSILON; // 数値誤差を避ける
        }
    }

    // TをAの座標系に変換
    T = { Dot(T, A[0]), Dot(T, A[1]), Dot(T, A[2]) };

    // 15軸について判定する（SAT）
    for (int i = 0; i < 3; ++i) {
        float ra = a.halfSize[i];
        float rb = b.halfSize[0] * AbsR[i][0] + b.halfSize[1] * AbsR[i][1] + b.halfSize[2] * AbsR[i][2];
        if (std::abs(T[i]) > ra + rb) return false;
    }

    for (int i = 0; i < 3; ++i) {
        float ra = a.halfSize[0] * AbsR[0][i] + a.halfSize[1] * AbsR[1][i] + a.halfSize[2] * AbsR[2][i];
        float rb = b.halfSize[i];
        if (std::abs(T[0] * R[0][i] + T[1] * R[1][i] + T[2] * R[2][i]) > ra + rb) return false;
    }

    // 斜め軸9本のチェック（省略可能な場合もあるが安全のためやる）
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            float ra = a.halfSize[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + a.halfSize[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
            float rb = b.halfSize[(j + 1) % 3] * AbsR[i][(j + 2) % 3] + b.halfSize[(j + 2) % 3] * AbsR[i][(j + 1) % 3];
            float t = std::abs(T[(i + 2) % 3] * R[(i + 1) % 3][j] - T[(i + 1) % 3] * R[(i + 2) % 3][j]);
            if (t > ra + rb) return false;
        }
    }

    return true;
}

void GamePlayScene::CheckBulletEnemyCollisionsOBB() {
    const auto& bullets = BulletManager::GetInstance()->GetPlayerBullets();

    for (const std::unique_ptr<PlayerBullet>& bullet : bullets) {
        if (!bullet->IsActive()) continue;

        for (auto& enemy : enemies_) {
            if (!enemy->IsActive()) continue;

            OBB bulletOBB = bullet->GetOBB(); // bulletがOBB情報を持っている必要あり
            OBB enemyOBB = enemy->GetOBB();   // 敵のOBBも同様に

            if (IsOBBIntersect(bulletOBB, enemyOBB)) {
                bullet->SetInactive();
                enemy->SetInactive();
                // パーティクル生成など
                break;
            }
        }
    }
}