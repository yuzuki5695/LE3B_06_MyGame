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
        
    TextureManager::GetInstance()->LoadTexture("Gameplay/Texture/Gage.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/Texture/Player_ui.png");
    

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
    
    gage_ = Sprite::Create("Gameplay/Texture/Gage.png", Vector2{ 380.0f, 10.0f }, 0.0f, Vector2{ 500.0f,30.0f });
    gage_->SetTextureSize(Vector2{ 500.0f,30.0f });     	 
    player_ui_ = Sprite::Create("Gameplay/Texture/Player_ui.png", Vector2{ 380.0f, 12.3f }, 0.0f, Vector2{ 25.0f,25.0f });
    player_ui_->SetTextureSize(Vector2{ 25.0f,25.0f });     

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
	MAX_ENEMY = 300; // 敵の最大数
    // 敵をリストに追加して初期化
    for (int i = 0; i < MAX_ENEMY; ++i) {
        std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
        enemy->Initialize();
        enemy->SetPlayer(player_.get());
        enemy->SetActive(false);  // 非アクティブにしておく
        enemies_.emplace_back(std::move(enemy));
    }
    // 敵出現トリガー
    enemySpawner_ = std::make_unique<EnemySpawner>(); enemySpawner_->Initialize(player_.get(), CameraManager::GetInstance(), &enemies_);

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
    if (!goal_ && player_->GetPosition().z >= CameraManager::GetInstance()->GetGameplayCamera()->GetBezierPoints().back().controlPoint.z && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        player_->SetKeyActive(false);
        player_->SetReticleVisible(false);
        // フェード開始             
        goal_ = true;
        end = false;
    }

    // ゲームスタートイベントが終了したらプレイヤ―操作可能に
    if (EventManager::GetInstance()->IsFinished() && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        //   イベント終了 → プレイヤーを操作可能に
        player_->SetKeyActive(true);
        player_->SetReticleVisible(true);
        // 進行度を設定
        StartStageProgressUI();
    }
    
    // ===== UI進行更新 =====
    UpdateStageProgressUI();

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
        enemySpawner_->Update();
        // 各衝突判定
        CheckBulletEnemyCollisionsOBB();
        CheckEnemyBulletPlayerCollisionsOBB(); 
        CheckEnemyPlayerCollisionsOBB();
        // 更新処理
        player_->Update();
        // プレイヤーがゴール手前までは敵も更新
        if (player_->GetPosition().z <=  CameraManager::GetInstance()->GetGameplayCamera()->GetBezierPoints().back().controlPoint.z) {
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

    gage_->Update();
    player_ui_->Update();
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
    if (player_->GetPosition().z <= CameraManager::GetInstance()->GetGameplayCamera()->GetBezierPoints().back().controlPoint.z) {
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

    gage_->Draw();
    player_ui_->Draw();

	// イベントマネージャの描画処理
    EventManager::GetInstance()->Draw2DSprite();
	// フェードマネージャの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}



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
                enemy->OnHit();
                
                particles_->AddHitPosition(enemy->GetPosition());
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
            end = true; // ゲームオーバーへ遷移など

            break;
        }
    }
}

void GamePlayScene::StartStageProgressUI() {
    if (uiProgressStarted_) return;
    uiStartRailLength_ = CameraManager::GetInstance()->GetGameplayCamera()->GetCurrentRailLength();

    uiProgressStarted_ = true;
}

void GamePlayScene::UpdateStageProgressUI() {
    if (!uiProgressStarted_ || uiProgressFinished_) return;

    auto* cam = CameraManager::GetInstance()->GetGameplayCamera();

    float current = cam->GetCurrentRailLength();
    float total   = cam->GetTotalRailLength();

    float uiTotalLength = total - uiStartRailLength_;
    if (uiTotalLength <= 0.0001f) return;

    float progress = (current - uiStartRailLength_) / uiTotalLength;

    if (progress >= 1.0f) {
        progress = 1.0f;
        uiProgressFinished_ = true;
    }

    progress = std::clamp(progress, 0.0f, 1.0f);

    constexpr float gageX = 380.0f;
    constexpr float gageWidth = 500.0f;
    constexpr float playerWidth = 25.0f;

    float movableWidth = gageWidth - playerWidth;
    float uiX = gageX + progress * movableWidth;

    player_ui_->SetPosition(Vector2{ uiX, 12.3f });
}
