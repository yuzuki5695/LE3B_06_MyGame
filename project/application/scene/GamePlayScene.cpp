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

    grass = Object3d::Create("terrain.obj", Transform({ {1.0f, 1.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 70.0f} }));
                                            
    // パーティクルグループ生成
    ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Resources/uvChecker.png", "plane.obj", VertexType::Model);            // モデルで生成

    // キャラクターの生成
    CharacterManager::GetInstance()->Initialize(); // リストクリア
    CharacterManager::GetInstance()->AddCharacter(std::make_unique<Player>());   // プレイヤーの登録
    for (int i = 0; i < 5; ++i) {
        CharacterManager::GetInstance()->AddCharacter(std::make_unique<Enemy>());    // 敵の登録
    }
    
    Player* player = CharacterManager::GetInstance()->GetPlayer();
    for (auto& character : CharacterManager::GetInstance()->GetCharacters()) {
        if (Enemy* enemy = dynamic_cast<Enemy*>(character.get())) {
            enemy->SetPlayer(player);
        }
    }

    CharacterManager::GetInstance()->InitializeAllCharacters(); // 登録クラスを全て初期化

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


    //if (CameraManager::GetInstance()->Getmovefige()) {
    //    //CheckBulletEnemyCollisions();  // 当たり判定(プレイヤーの球と敵)
    //    //CheckEnemyBulletPlayerCollisions(); // 当たり判定(プレイヤーと敵の弾)
    //    //CheckPlayerEnemyCollisions();  // 当たり判定(プレイヤーと敵)
    //    //CleanupInactiveObjects();      // 不要なオブジェクト削除
    //}

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

    //// 自弾リストの取得
    //const std::vector<Bullet*>& bullets_ = player_->GetBullets();


    //for (Bullet* bullet : bullets_) {
    //    if (!bullet->IsActive()) continue;

    //    for (const auto& enemy : enemys_) {
    //        if (!enemy->IsActive()) continue;

    //        //float dist = Length(bullet->GetPosition() - enemy->GetPosition());
    //        Vector3 delta = {
    //            bullet->GetPosition().x - enemy->GetPosition().x
    //            , bullet->GetPosition().y - enemy->GetPosition().y
    //            , bullet->GetPosition().z - enemy->GetPosition().z
    //        };
    //        float dist = Length(delta);

    //        float collisionDist = bullet->GetRadius() + enemy->GetRadius();

    //        if (dist <= collisionDist) {
    //            bullet->SetInactive(); // 弾を非アクティブに
    //            enemy->SetInactive();  // 敵も非アクティブに


    //            // 敵の位置にパーティクルを出す
    //            Transform particleTransform = {
    //                {1.0f, 2.0f, 1.0f},                  // scale（必要に応じて変更）
    //                {0.0f, 0.0f, 0.0f},                  // rotation
    //                {enemy->GetPosition().x,enemy->GetPosition().y + 2.0f,enemy->GetPosition().z}                // 敵の位置を使用
    //            };
    //            break; // 一発で1体だけ倒す場合はbreak
    //        }
    //    }
    //}
}

void GamePlayScene::CleanupInactiveObjects() {
    //// 自弾リストの取得
    //std::vector<Bullet*>& bullets_ = player_->GetBullet();

    //// bullets_ を非アクティブなものだけ削除
    //bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
    //    [](Bullet* b) {
    //        if (!b->IsActive()) {
    //            delete b;
    //            return true;
    //        }
    //        return false;
    //    }), bullets_.end());

    //enemys_.erase(std::remove_if(enemys_.begin(), enemys_.end(),
    //    [](const std::unique_ptr<Enemy>& e) {
    //        return !e->IsActive(); // unique_ptr の所有権は move されるので delete は不要
    //    }), enemys_.end());

}

void GamePlayScene::CheckPlayerEnemyCollisions() {
    //const Vector3& playerPos = player_->GetObject3d()->GetTransform().translate;
    //float playerRadius = player_->GetObject3d()->GetTransform().scale.x * 0.5f; // 半径 = スケールの半分

    //for (const auto& enemy : enemys_) {
    //    if (!enemy->IsActive()) continue;

    //    const Vector3& enemyPos = enemy->GetPosition();
    //    float enemyRadius = enemy->GetRadius();

    //    Vector3 delta = {
    //        playerPos.x - enemyPos.x,
    //        playerPos.y - enemyPos.y,
    //        playerPos.z - enemyPos.z
    //    };
    //    float dist = Length(delta);
    //    float collisionDist = playerRadius + enemyRadius;

    //    if (dist <= collisionDist) {
    //        if (!player_->IsActive()) {
    //            // 敵を無効化
    //            enemy->SetInactive();
    //        }

    //        // プレイヤーを無効化 ← ここを追加！
    //        player_->SetInactive();

    //        // パーティクル発生
    //        Transform particleTransform = {
    //            {0.05f, 1.0f, 1.0f},
    //            {0.0f, 0.0f, 0.0f},
    //            {enemy->GetPosition().x, enemy->GetPosition().y + 2.0f, enemy->GetPosition().z}
    //        };

    //        // break でループ終了（1体と当たったら終わる）
    //        break;
    //    }
    //}
}

void GamePlayScene::CheckEnemyBulletPlayerCollisions() {
    //if (!player_->IsActive()) return;

    //const Vector3& playerPos = player_->GetObject3d()->GetTransform().translate;
    //float playerRadius = player_->GetObject3d()->GetTransform().scale.x * 0.5f; // 半径 = スケールの半分


    //for (const auto& enemy : enemys_) {
    //    const std::vector<Bullet*>& enemyBullets = enemy->GetBullets();

    //    for (Bullet* bullet : enemyBullets) {
    //        if (!bullet->IsActive()) continue;

    //        Vector3 delta = {
    //            bullet->GetPosition().x - playerPos.x,
    //            bullet->GetPosition().y - playerPos.y,
    //            bullet->GetPosition().z - playerPos.z
    //        };
    //        float dist = Length(delta);

    //        float collisionDist = bullet->GetRadius() + playerRadius;

    //        if (dist <= collisionDist) {
    //            if (!player_->IsActive()) {
    //                bullet->SetInactive();    // 弾を消す
    //            }
    //            player_->SetInactive();   // プレイヤーを消す

    //            // 必要ならエフェクト生成も追加
    //            break; // 一発で終わらせるなら break
    //        }
    //    }
    //}
}
