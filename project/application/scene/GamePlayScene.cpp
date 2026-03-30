#include "GamePlayScene.h"
#include <SceneManager.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <CameraManager.h>
#include <ParticleCommon.h>
#include <Input.h>
#include <Controller.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <SkyboxCommon.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include <Collision.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <Easing.h>
#include <UIManager.h>
#include <GamePlayUI.h>
#include <StageManager.h>

// 各種マネージャやユーティリティの名前空間を使用
using namespace LoadResourceID;
using namespace Collision;
using namespace MatrixVector;
using namespace Easing;

void GamePlayScene::Finalize() {
    BulletManager::GetInstance()->Finalize();  // 弾の解放処理
    FadeManager::GetInstance()->Finalize();    //  フェードマネージャの解放処理
    EventManager::GetInstance()->Finalize();   //  イベントマネージャの解放処理
    StageManager::GetInstance()->Finalize();   // ステージマネージャの解放処理
	UIManager::GetInstance()->Finalize();      // UIマネージャの解放処理
}

void GamePlayScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    CameraManager::GetInstance()->SetTypeview(ViewCameraType::Main);

    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel(model::Goal);

    // プレイヤーの作成と初期化
    player_ = std::make_unique<Player>();
    player_->Initialize(); // プレイヤーの初期化
    CameraManager::GetInstance()->SetGamecameraTarget(player_->GetPlayerObject());
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
    wall = Object3d::Create(model::Goal, Transform{ { 2.0f, 2.0f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 8.0f, 39.0f, 800.0f } });
    // スカイボックスの作成
    TextureManager::GetInstance()->LoadTexture(texture::Cubemapbox);
    Box_ = Skybox::Create(texture::Cubemapbox, Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });
    // ゴールフラグ初期化
    goal_ = false;
    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();
    // イベントマネージャの初期化
    EventManager::GetInstance()->Initialize("gamestart");
    // ステージマネージャの初期化
    StageManager::GetInstance()->Initialize();

    end = false;
    // ポーズメニューの初期化
    pausemenu_ = std::make_unique<Pausemenu>();
    pausemenu_->Initialize();
    isPausedevent_ = false;

    // UIマネージャの初期化
    UIManager::GetInstance()->Initialize();
    // 3. UIManagerから GamePlayUI を探してプレイヤーを渡す
    GamePlayUI* gpUI = UIManager::GetInstance()->GetUI<GamePlayUI>();
    if (gpUI) {
        gpUI->SetPlayer(player_.get());
    }
}
void GamePlayScene::PauseMenuUpdate() {
    //  Enterキーでポーズの「開始」のみをチェック
    if (isPausedevent_ && !pausemenu_->IsActive() && Input::GetInstance()->Triggrkey(DIK_TAB)) {
        pausemenu_->SetActive(true); // 演出開始！
    }
    pausemenu_->Update();
}

void GamePlayScene::Update() {       
    // ---------------------------
    // ポーズメニュー処理
    // ---------------------------
	// ポーズメニューの更新
    PauseMenuUpdate();
    if (pausemenu_->IsActive()) {
        HandlePauseMenuCommands();  // ポーズ中のコマンド処理
        return;                     // ポーズ中はその他の処理をスキップ
    }
    // ---------------------------
    // フェード処理の開始
    // ---------------------------
    HandleFadeInOut();
    // ---------------------------
    // イベントマネージャ更新
    // ---------------------------
    EventManager::GetInstance()->Update(); 	
    // ---------------------------
    // プレイヤーの状態に応じた処理
    // ---------------------------
    HandlePlayerState();       
    // ---------------------------
    // ステージ更新
    // ---------------------------
    StageManager::GetInstance()->Update();
    // ---------------------------
    // カメラの更新処理
    // ---------------------------
    // サブカメラはプレイヤーを追わせる
    CameraManager::GetInstance()->SetGamecameraTarget(player_->GetPlayerObject());
    CameraManager::GetInstance()->Update();
#pragma region 全てのObject3d個々の更新処理

    // 終了しない限り更新処理
    if (!end) {
        if (EventManager::GetInstance()->IsFinished()) {
            // 敵出現動作
            enemySpawner_->Update();
        }
        
        // 各衝突判定
        CheckBulletEnemyCollisionsOBB();
        CheckEnemyBulletPlayerCollisionsOBB(); 
        CheckEnemyPlayerCollisionsOBB();
        wall->Update();

    }
    // 更新処理
    player_->Update();
    
    // プレイヤーがゴール手前までは敵も更新
    if (player_->GetPosition().z <= CameraManager::GetInstance()->GetGameplayCamera()->GetBezierPoints().back().controlPoint.z) {
        // 敵の更新
        for (auto& enemy : enemies_) {
            if (enemy->IsActive()) {
                enemy->SetPlayer(player_.get());
                enemy->Update();
            }
        }
    }

    if (!end) {
        // Bulletマネージャの更新処理
        BulletManager::GetInstance()->Update();
    }
     
    // 敵がプレイヤーから離れすぎたら削除（過去の敵掃除）
    for (auto& enemy : enemies_) {
        if (!enemy->IsActive()) continue;
        float playerZ = player_->GetPosition().z;
        float spawnZ = enemy->GetSpawnBaseZ();  // 出現基準Z
        if (playerZ > spawnZ + 30) { // 出現位置より進んでたら削除
            enemy->Kill();
        }
    }

    // フェードアウトが完了したら次のシーンへ
    if (FadeManager::GetInstance()->IsFadeEnd() && FadeManager::GetInstance()->GetFadeType() == FadeType::FadeOut) {
        if (player_->GetState() == State::Dead) {
            // シーン切り替え
            SceneManager::GetInstance()->ChangeScene("GAMEOVER");
        } else if (player_->GetState() == State::Goal) {
            // シーン切り替え
            SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
        }
    }

    // スカイボックス更新
    Box_->Update(); 

    for (auto& enemy : enemies_) {
        enemy->onDeathCallback = [this](const Vector3& pos) {
            particles_->AddHitPosition(pos);
            };
    }
    // パーティクル更新
    ParticleManager::GetInstance()->Update();
    particles_->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理

    // UIマネージャの更新
    UIManager::GetInstance()->Update();
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

    player_->Draw();
    
    // 描画処理    
    if (!end) {
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
    }
    wall->Draw();
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

    pausemenu_->Draw();
  

    if (CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        UIManager::GetInstance()->Draw();
    }

    // イベントマネージャの描画処理
    EventManager::GetInstance()->Draw2DSprite();
    // フェードマネージャの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}

void GamePlayScene::HandlePauseMenuCommands() {
    if (pausemenu_->GetCommand() == PauseCommand::GoToTitle) {
        FadeManager::GetInstance()->Update();
        if (!goal_) {
            FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
            goal_ = true;
        }
        if (FadeManager::GetInstance()->IsFadeEnd()) {
            SceneManager::GetInstance()->ChangeScene("TITLE");
        }
    }
}

void GamePlayScene::HandleFadeInOut() {
    if (!FadeManager::GetInstance()->IsFadeStart() && !FadeManager::GetInstance()->IsFading()) {
        FadeManager::GetInstance()->StartFadeIn(1.0f, FadeStyle::SilhouetteExplode);
    }
    FadeManager::GetInstance()->Update();
}

void GamePlayScene::HandlePlayerState() {
    // 死亡演出
    if (player_->GetState() == State::Dead && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        CameraManager::GetInstance()->SetMode(CameraMode::Default);
        CameraManager::GetInstance()->SetTypeview(ViewCameraType::Sub);
        end = false;
    }

    // サブカメラ時の処理
    if (CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Sub && CameraManager::GetInstance()->GetMode() == CameraMode::Default) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        CameraManager::GetInstance()->SetMode(CameraMode::Follow);
    }

    // ゴール判定
    if (!goal_ && player_->GetState() == State::Alive && player_->GetPosition().z >= CameraManager::GetInstance()->GetGameplayCamera()->GetBezierPoints().back().controlPoint.z) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        player_->SetState(State::Goal);
        goal_ = true;
        end = false;
    }

    // ゲーム開始後にプレイヤー操作可能に
    if (player_->GetState() == State::None && EventManager::GetInstance()->IsFinished()) {
        player_->SetState(State::Alive);
        isPausedevent_ = true;
        if (auto gpUI = UIManager::GetInstance()->GetUI<GamePlayUI>()) {
            gpUI->StartStageProgressUI();
        }
    }

    // 操作UIのアニメーション開始
    if (EventManager::GetInstance()->GetProgress() >= 2.5f) {
        if (auto gpUI = UIManager::GetInstance()->GetUI<GamePlayUI>()) {
            gpUI->StartControlUIAnimation();
        }
    }
   
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
                
               // particles_->AddHitPosition(enemy->GetPosition());
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
    // プレイヤーがダッシュ中なら、当たり判定を一切行わずに終了する
    if (player_->GetDash()->IsDashing()) {
        return; 
    }

    const auto& bullets = BulletManager::GetInstance()->GetEnemyBullets();

    OBB playerOBB = player_->GetOBB(); // プレイヤーがOBBを返すようにしておく必要あり

    for (const std::unique_ptr<EnemyBullet>& bullet : bullets) {
        if (!bullet->IsActive()) continue;

        OBB bulletOBB = bullet->GetOBB(); // 弾にもOBBが必要

        if (IsOBBIntersect(bulletOBB, playerOBB)) {
            bullet->SetInactive();
            player_->SetState(State::Dead);

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
    // プレイヤーがダッシュ中なら、当たり判定を一切行わずに終了する
    if (player_->GetDash()->IsDashing()) {
        return;
    }

    // プレイヤーのOBBを取得
    OBB playerOBB = player_->GetOBB();

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive() || enemy->IsDead()) continue;

        // 敵のOBBを取得
        OBB enemyOBB = enemy->GetOBB();

        // 衝突判定
        if (IsOBBIntersect(playerOBB, enemyOBB)) {
            enemy->OnHit();
            player_->SetState(State::Dead);
            end = true; // ゲームオーバーへ遷移など
            break;
        }
    }
}
