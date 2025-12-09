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
#include<Collision.h>

using namespace Collision;
using namespace MatrixVector;

///====================================================
/// 終了処理（リソース解放）
///====================================================
void GamePlayScene::Finalize() {
    BulletManager::GetInstance()->Finalize();  // 弾の解放処理
    FadeManager::GetInstance()->Finalize();    //  フェードマネージャの解放処理
    EventManager::GetInstance()->Finalize();   //  イベントマネージャの解放処理
    StageManager::GetInstance()->Finalize();		  // ステージマネージャの解放処理
}
///====================================================
/// 初期化処理
///====================================================
void GamePlayScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));     
    CameraManager::GetInstance()->SetTypeview(ViewCameraType::Main);

    // テクスチャを読み込む
    TextureManager::GetInstance()->LoadTexture("Gameplay/Move.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/Space.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/Shift.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/StandardChange.png");     
    TextureManager::GetInstance()->LoadTexture("titlereturn.png");
    TextureManager::GetInstance()->LoadTexture("titlereturn02.png");
        
    TextureManager::GetInstance()->LoadTexture("Gameplay/Texture/UI_01.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/Texture/UI_02.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/Texture/UI_03.png");

    MAXui_ = 1;
    uis_.push_back(Sprite::Create("Gameplay/Texture/UI_01.png", Vector2{ 8.0f, 430.0f }, 0.0f, Vector2{ 100.0f,80.0f })); 
    uis_.push_back(Sprite::Create("Gameplay/Texture/UI_02.png", Vector2{ 8.0f, 530.0f }, 0.0f, Vector2{ 100.0f,80.0f })); 
    uis_.push_back(Sprite::Create("Gameplay/Texture/UI_03.png", Vector2{ 8.0f, 630.0f }, 0.0f, Vector2{ 100.0f,80.0f })); 
    uis_[0]->SetTextureSize(Vector2{100.0f,80.0f});
    uis_[1]->SetTextureSize(Vector2{100.0f,80.0f});
    uis_[2]->SetTextureSize(Vector2{100.0f,80.0f});

    // タイトルに戻るUIを生成
    ui1_ = Sprite::Create("titlereturn02.png", Vector2{ 1100.0f, 5.0f }, 0.0f, Vector2{ 150.0f,100.0f });
    ui1_->SetTextureSize(Vector2{ 300.0f,200.0f });

    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel("terrain.obj");
    ModelManager::GetInstance()->LoadModel("monsterBallUV.obj");
    ModelManager::GetInstance()->LoadModel("Bullet/PlayerBullet.obj");
    ModelManager::GetInstance()->LoadModel("Bullet/EnemyBullet.obj");
    ModelManager::GetInstance()->LoadModel("Gameplay/Model/Goal/Goal.obj");

    // プレイヤーの作成と初期化
    player_ = std::make_unique<Player>();
    player_->Initialize(); // プレイヤーの初期化
    CameraManager::GetInstance()->SetGamecameraTarget(player_->GetPlayerObject());
    playerhp_ = player_->IsActive();
    // カメラにプレイヤーを追わせる

    // パーティクル
    particles_ = std::make_unique<GamePlayparticle>();
    particles_->Initialize(player_->GetPlayerObject());

    // 敵関連の初期化
	MAX_ENEMY = 20; // 敵の最大数
    // 敵出現トリガー
    spawnTriggers_ = {
    {Vector3{0.0f,0.0f,150.0f}, 5, false, MoveType::Horizontal},       // 全部動かない None(フォーメーション関数使用中)
    {Vector3{0.0f,0.0f,250.0f}, 5, false, MoveType::Vertical},        // 全部縦移動 Vertical
    {Vector3{0.0f,0.0f,350.0f}, 5, false, MoveType::None},             // 全部横移動  Horizontal
    };

    // 敵をリストに追加して初期化
    for (int i = 0; i < MAX_ENEMY; ++i) {
        std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
        enemy->Initialize();
        enemy->SetPlayer(player_.get());
        enemy->SetActive(false);  // 非アクティブにしておく
        enemies_.emplace_back(std::move(enemy));
    }
    // クリアゲート(仮)
    wall = Object3d::Create("Gameplay/Model/Goal/Goal.obj", Transform{ { 2.0f, 2.0f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 2.0f, 500.0f } });
    // スカイボックスの作成
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
    Box_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });
    // ゴールフラグ初期化
    goal_ = false;
    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();
    // イベントマネージャの初期化
    EventManager::GetInstance()->Initialize("gamestart");
	// ゲームカメラの移動許可
   // CameraManager::GetInstance()->GetGameCamera()->Setmovefige(true);

	goalpos_ = 500.0f;
    // ステージマネージャの初期化
    StageManager::GetInstance()->Initialize();

    end = false;
}
///====================================================
/// 毎フレーム更新処理
///====================================================
void GamePlayScene::Update() {
    // フェードイン開始
    if (!FadeManager::GetInstance()->IsFadeStart() && !FadeManager::GetInstance()->IsFading()) {
        // フェード開始
        FadeManager::GetInstance()->StartFadeIn(1.0f, FadeStyle::SilhouetteExplode);
    }
    // フェードマネージャの更新   
    FadeManager::GetInstance()->Update();
    // イベントマネージャの更新
    EventManager::GetInstance()->Update(); 	

    // 死亡演出
    if (end && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        CameraManager::GetInstance()->SetMode(CameraMode::Default);   
        CameraManager::GetInstance()->SetTypeview(ViewCameraType::Sub);
        player_->SetKeyActive(false);
        player_->SetReticleVisible(false);
        // フェード開始             
        end = false;
    }

    if (CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Sub && CameraManager::GetInstance()->GetMode() == CameraMode::Default) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        CameraManager::GetInstance()->SetMode(CameraMode::Follow);
    }

    // ゴール演出
    if (player_->GetPosition().z >= goalpos_ && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        player_->SetKeyActive(false);
        player_->SetReticleVisible(false);
        goalpos_ = 1000.0f;
        // フェード開始             
        end = false;
    }

    // ゲームスタートイベントが終了したらプレイヤ―操作可能に
    if (EventManager::GetInstance()->IsFinished() && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        //   イベント終了 → プレイヤーを操作可能に
        player_->SetKeyActive(true);
        player_->SetReticleVisible(true);
    }

        
    //// フェードアウトが完了したら次のシーンへ
    //if (!end && Input::GetInstance()->Triggrkey(DIK_RETURN)) {
    //    player_->SetInactive();
    //    //CameraManager::GetInstance()->SetMode(CameraMode::Transition);
    //    //player_->SetKeyActive(false);
    //    //player_->SetDead_(true);
    //    end =true;
    //}

    StageManager::GetInstance()->Update();

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    // サブカメラはプレイヤーを追わせる
    CameraManager::GetInstance()->SetGamecameraTarget(player_->GetPlayerObject());
    CameraManager::GetInstance()->Update();
#pragma region 全てのObject3d個々の更新処理
    playerhp_ = player_->IsActive();
    // 終了しない限り更新処理
    if (!end) {
        // 敵出現動作
        EnemySpawn();
        // 各衝突判定
        CheckBulletEnemyCollisionsOBB();
        CheckEnemyBulletPlayerCollisionsOBB(); 
        CheckEnemyPlayerCollisionsOBB();
        // 更新処理
        player_->Update();
        // プレイヤーがゴール手前なら敵も更新
        if (player_->GetPosition().z <= goalpos_) {
            // 敵の更新
            for (auto& enemy : enemies_) {
                if (enemy->IsActive()) {
                    enemy->SetPlayer(player_.get());
                    enemy->Update();
                }
            }
        }
        wall->Update();
        // Bulletマネージャの更新処理
        BulletManager::GetInstance()->Update();
    }

    // 敵がプレイヤーから離れすぎたら削除（過去の敵掃除）
    for (auto& enemy : enemies_) {
        if (!enemy->IsActive()) continue;
        float playerZ = player_->GetPosition().z;
        float spawnZ = enemy->GetSpawnBaseZ();  // 出現基準Z
        if (playerZ > spawnZ + 10) { // 出現位置より進んでたら削除
            enemy->Kill();
        }
    }

    // フェードアウトが完了したら次のシーンへ
    if (FadeManager::GetInstance()->IsFadeEnd() && FadeManager::GetInstance()->GetFadeType() == FadeType::FadeOut) {
        if (!playerhp_) {
            // シーン切り替え
            SceneManager::GetInstance()->ChangeScene("GAMEOVER");
        } else {
            // シーン切り替え
            SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
        }
    }

    // 死んだ敵の削除
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& e) {
                return e->IsDead();  // ← 出現前の非アクティブは残す！
            }),
        enemies_.end());
    // スカイボックス更新
    Box_->Update();     
    // パーティクル更新
    ParticleManager::GetInstance()->Update();
    particles_->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理
    // スプライトの更新
    ui1_->Update();

    for (std::unique_ptr<Sprite>& ui : uis_) {
        ui->Update();
    }

#pragma endregion 全てのSprite個々の更新処理

#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI 
    ImGui::Begin("=== GamePlayScene Debug ===");
    // 「end」フラグを切り返すチェックボックス
    ImGui::Checkbox("End Flag", &end);
    ImGui::End();

  //  Object3dCommon::GetInstance()->DrawImGui(); // object3dのlightのImGui制御
    CameraManager::GetInstance()->DrawImGui();  // カメラマネージャのImGui制御
//	FadeManager::GetInstance()->DrawImGui();    // フェードマネージャのImGui制御 
///	EventManager::GetInstance()->DrawImGui();   // イベントマネージャのImGui制御
	//stageManager_->DebugImGui(); 			  // ステージマネージャのImGui制御
#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了 
}
///====================================================
/// 描画処理
///====================================================
void GamePlayScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    Box_->Draw();
    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing(); 
    StageManager::GetInstance()->Draw();

    // 描画処理
    if (!end) {
        player_->Draw();
        // Bulletマネージャの描画処理
        BulletManager::GetInstance()->Draw();
    }
    // プレイヤーがゴール地点に達するまでは敵や壁を描画
    if (player_->GetPosition().z <= goalpos_) {
        // 敵の更新
        for (auto& enemy : enemies_) {
            if (enemy->IsActive()) {
                enemy->Draw();
            }
        }
        wall->Draw();
    }

	// イベントマネージャの描画処理
    EventManager::GetInstance()->Drawo3Dbject();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
    ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();
    // プレイヤーのUIスプライト描画
    if (!end) {
        player_->DrawSprite();
    }

    // スタートイベント終了後にタイトルへ戻るボタンを描画   
    if (EventManager::GetInstance()->IsFinished()) {
        //ui1_->Draw();
    }
    
    for (std::unique_ptr<Sprite>& ui : uis_) {
        ui->Draw();
    }

	// イベントマネージャの描画処理
    EventManager::GetInstance()->Draw2DSprite();
	// フェードマネージャの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}
///====================================================
/// 敵の出現処理
///====================================================
void GamePlayScene::EnemySpawn() {
    // プレイヤーの位置取得
    Vector3 playerPos = player_->GetPosition(); 
    GamePlayCamera* gameCam = CameraManager::GetInstance()->GetGameplayCamera(); 

    if (!gameCam) return;
    // プレイヤー（カメラ）の方向ベクトルを取得
    Vector3 forward = gameCam->GetForward();
    Vector3 right = Normalize(Cross({ 0,1,0 }, forward));
    Vector3 up = Normalize(Cross(forward, right));

    for (size_t tIndex = 0; tIndex < spawnTriggers_.size(); ++tIndex) {
        auto& trigger = spawnTriggers_[tIndex];
        // Z位置比較ではなく「進行方向距離」で判定
        float distanceForward = Dot((trigger.Position - playerPos), forward);
        if (!trigger.hasSpawned && distanceForward >= 0.0f && distanceForward < 10.0f) {
            // --- 出現パターン分岐 ---
            switch (tIndex) {
            case 0: SpawnZigZagFormation(trigger); break;
            case 1: SpawnZigZagFormation(trigger); break;
            case 2: SpawnZigZagFormation(trigger); break;
            }
            trigger.hasSpawned = true;
        }
    }
}
///====================================================
/// 敵出現パターン：ジグザグフォーメーション
///====================================================
void GamePlayScene::SpawnZigZagFormation(const EnemySpawnTrigger& trigger) {
    // プレイヤー・カメラ情報を取得
    Vector3 playerPos = player_->GetPosition();
    GamePlayCamera* gameCam = CameraManager::GetInstance()->GetGameplayCamera(); 
    if (!gameCam) return;

    Vector3 forward = Normalize(gameCam->GetForward());
    Vector3 right   = Normalize(Cross({ 0,1,0 }, forward));
    Vector3 up      = Normalize(Cross(forward, right));

    // 敵配置設定
    int activated = 0;
    const int numEnemies = 5;
    const float forwardDist = 50.0f;
    const float baseY = 5.0f;

    const float xOffsets[5] = { -1.5f,  1.5f,  0.0f, -1.5f,  1.5f };
    const float yOffsets[5] = {  2.0f,  2.0f,  0.0f, -2.0f, -2.0f };

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive() && activated < numEnemies) {
            float x = xOffsets[activated];
            float y = yOffsets[activated];

            Vector3 spawnPos =
                playerPos +
                forward * forwardDist +
                right * x +
                up * (baseY + y);

            enemy->SetnewTranslate(spawnPos, trigger.moveType);
            enemy->SetActive(true);
            ++activated;
        }
    }
}
/////====================================================
///// 敵出現パターン：逆ステップフォーメーション（姿勢対応）
/////====================================================
//void GamePlayScene::SpawnReverseStepFormation(const EnemySpawnTrigger& trigger) {
//    // プレイヤー・カメラ情報を取得
//    Vector3 playerPos = player_->GetPosition();
//    GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
//    if (!gameCam) return;
//
//    Vector3 forward = Normalize(gameCam->GetForward());
//    Vector3 right   = Normalize(Cross({ 0,1,0 }, forward));
//    Vector3 up      = Normalize(Cross(forward, right));
//
//    // 敵配置設定
//    int activated = 0;
//    const float baseY = 5.0f;
//    const float forwardBase = 80.0f;
//    const float stepX = 1.5f;
//    const float stepY = -1.2f;
//    const float stepZ = 5.0f;
//
//    for (auto& enemy : enemies_) {
//        if (!enemy->IsActive()) {
//            Vector3 spawnPos =
//                playerPos +
//                forward * (forwardBase - stepZ * activated) +
//                right   * -(stepX * activated) +
//                up      * (baseY + stepY * activated);
//
//            enemy->SetnewTranslate(spawnPos, trigger.moveType);
//            enemy->SetActive(true);
//
//            ++activated;
//            if (activated >= trigger.spawnCount) break;
//        }
//    }
//}
/////====================================================
///// 敵出現パターン：V字フォーメーション（姿勢対応）
/////====================================================
//void GamePlayScene::SpawnVFormation(const EnemySpawnTrigger& trigger) {
//    // プレイヤー・カメラ情報を取得
//    Vector3 playerPos = player_->GetPosition();
//    GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
//    if (!gameCam) return;
//
//    Vector3 forward = Normalize(gameCam->GetForward());
//    Vector3 right   = Normalize(Cross({ 0,1,0 }, forward));
//    Vector3 up      = Normalize(Cross(forward, right));
//
//    // 敵配置設定
//    int activated = 0;
//
//    const float forwardDist = 60.0f;  // 前方距離
//    const float baseY = 1.0f;
//    const float stepX = 3.0f;
//    const float stepY = -2.0f;
//
//    for (auto& enemy : enemies_) {
//        if (!enemy->IsActive()) {
//            float x = 0.0f;
//            float y = 0.0f;
//
//            switch (activated) {
//            case 0: x =  0.0f; y =  0.0f; break;               // 中央
//            case 1: x = -stepX; y = stepY; break;              // 左上
//            case 2: x =  stepX; y = stepY; break;              // 右上
//            case 3: x = -stepX * 2; y = stepY * 2; break;      // 左下
//            case 4: x =  stepX * 2; y = stepY * 2; break;      // 右下
//            default:
//                x = (activated - 2) * stepX;
//                y = stepY * 3;
//                break;
//            }
//
//            Vector3 spawnPos =
//                playerPos +
//                forward * forwardDist +
//                right * x +
//                up * (baseY + y);
//
//            enemy->SetnewTranslate(spawnPos, trigger.moveType);
//            enemy->SetActive(true);
//
//            ++activated;
//            if (activated >= trigger.spawnCount) break;
//        }
//    }
//}
//

///====================================================
/// プレイヤー弾 vs 敵の当たり判定
///====================================================
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
///====================================================
/// 敵弾 vs プレイヤーの当たり判定
///====================================================
void GamePlayScene::CheckEnemyBulletPlayerCollisionsOBB() {
    const auto& bullets = BulletManager::GetInstance()->GetEnemyBullets();

    if (!player_ || !player_->IsActive()) return;

    OBB playerOBB = player_->GetOBB(); // プレイヤーがOBBを返すようにしておく必要あり

    for (const std::unique_ptr<EnemyBullet>& bullet : bullets) {
        if (!bullet->IsActive()) continue;

        OBB bulletOBB = bullet->GetOBB(); // 弾にもOBBが必要

        if (IsOBBIntersect(bulletOBB, playerOBB)) {
            bullet->SetInactive();
            player_->SetInactive();  // プレイヤーを無効にする

            end = true;
            // ヒットエフェクトなど追加
            break;
        }
    }
}
///====================================================
/// 敵 vs プレイヤー の当たり判定
///====================================================
void GamePlayScene::CheckEnemyPlayerCollisionsOBB() {
    if (!player_ || !player_->IsActive()) return;

    // プレイヤーのOBBを取得
    OBB playerOBB = player_->GetOBB();

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive() || enemy->IsDead()) continue;

        // 敵のOBBを取得
        OBB enemyOBB = enemy->GetOBB();

        // 衝突判定
        if (IsOBBIntersect(playerOBB, enemyOBB)) {
            player_->SetInactive();
            enemy->SetInactive();
            end = true; // ゲームオーバーへ遷移など

            break;
        }
    }
}
