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

///====================================================
/// 終了処理（リソース解放）
///====================================================
void GamePlayScene::Finalize() {
    BulletManager::GetInstance()->Finalize();  // 弾の解放処理
    FadeManager::GetInstance()->Finalize();    //  フェードマネージャの解放処理
    EventManager::GetInstance()->Finalize();   //  イベントマネージャの解放処理
	stageManager_->Finalize();			  // ステージマネージャの解放処理
}
///====================================================
/// 初期化処理
///====================================================
void GamePlayScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    CameraManager::GetInstance()->SetCameraMode(CameraMode::GamePlay);

    // テクスチャを読み込む
    TextureManager::GetInstance()->LoadTexture("uvChecker.png");
    TextureManager::GetInstance()->LoadTexture("monsterBall.png");
    TextureManager::GetInstance()->LoadTexture("fade/Black.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/Move.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/Space.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/Shift.png");
    TextureManager::GetInstance()->LoadTexture("Gameplay/StandardChange.png");     
    TextureManager::GetInstance()->LoadTexture("titlereturn.png");

    // タイトルに戻るUIを生成
    ui1_ = Sprite::Create("titlereturn.png", Vector2{ 1100.0f, 5.0f }, 0.0f, Vector2{ 150.0f,100.0f });
    ui1_->SetTextureSize(Vector2{ 300.0f,200.0f });

    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel("terrain.obj");
    ModelManager::GetInstance()->LoadModel("monsterBallUV.obj");
    ModelManager::GetInstance()->LoadModel("Bullet/PlayerBullet.obj");
    ModelManager::GetInstance()->LoadModel("EnemyBullet.obj");
    ModelManager::GetInstance()->LoadModel("Clear.obj");
    ModelManager::GetInstance()->LoadModel("wall.obj");

    // プレイヤーの作成と初期化
    player_ = std::make_unique<Player>();
    player_->Initialize(); // プレイヤーの初期化
    playerhp_ = player_->IsActive();
    // カメラにプレイヤーを追わせる
    CameraManager::GetInstance()->SetTarget(player_->GetPlayerObject());

    // 敵関連の初期化
	MAX_ENEMY = 14; // 敵の最大数
    // 敵出現トリガー
    spawnTriggers_ = {
    {40.0f, 5, false, MoveType::Horizontal},       // 全部動かない None(フォーメーション関数使用中)
    {120.0f, 5, false, MoveType::Vertical},        // 全部縦移動 Vertical
    {190.0f, 5, false, MoveType::None}             // 全部横移動  Horizontal
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
    wall = Object3d::Create("wall.obj", Transform{ { 10.0f, 0.7f, 0.7f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 300.0f } });
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
    CameraManager::GetInstance()->GetGameCamera()->Setmovefige(true);

	goalpos_ = 280.0f;

	stageManager_ = std::make_unique<StageManager>();
	stageManager_->Initialize();
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

    if (end && CameraManager::GetInstance()->GetGameCamera()->GetMode() == ViewType::Main) {
        CameraManager::GetInstance()->GetGameCamera()->SwitchView(ViewType::Sub);
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        // フェード開始             
        end = false;
    }

    if (CameraManager::GetInstance()->GetGameCamera()->GetMode() == ViewType::Sub) {
        player_->SetDead_(true);
    }

    // ゲームスタートイベントが終了したらプレイヤ―操作可能に
    if (EventManager::GetInstance()->IsFinished()) {
        //   イベント終了 → プレイヤーを操作可能に
        player_->SetKeyActive(true);
        player_->SetReticleVisible(true);
        if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
            end = true;
        }
    }

    stageManager_->Update();

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();
    CameraManager::GetInstance()->GetGameCamera()->SetFollowTarget(player_->GetPlayerObject());
#pragma region 全てのObject3d個々の更新処理
    playerhp_ = player_->IsActive();
    // 終了しない限り更新処理
//    if (!end) {
        //// 敵出現動作
        //EnemySpawn();
        //// 各衝突判定
        //CheckBulletEnemyCollisionsOBB();
        //CheckEnemyBulletPlayerCollisionsOBB();
        // 更新処理
        player_->Update();
        // プレイヤーがゴール手前なら敵も更新
        if (player_->GetPosition().z <= goalpos_) {
            // 敵の更新
            for (auto& enemy : enemies_) {
                if (enemy->IsActive()) {
                    enemy->Update();
                }
            }
        }
        wall->Update();
        // Bulletマネージャの更新処理
        BulletManager::GetInstance()->Update();
  //  }

    // 敵がプレイヤーから離れすぎたら削除（過去の敵掃除）
    for (auto& enemy : enemies_) {
        if (!enemy->IsActive()) continue;
        float playerZ = player_->GetPosition().z;
        float spawnZ = enemy->GetSpawnBaseZ();  // 出現基準Z
        if (playerZ > spawnZ + 10) { // 出現位置より進んでたら削除
            enemy->Kill();
        }
    }
    // ゴール判定処理
    if (player_->GetPosition().z >= goalpos_ && !end) {
        end = true;
        goal_ = true;
    }

	// ゴールしたらフェードアウト開始
    if (goal_) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        goal_ = false;
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
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理
    // スプライトの更新
    ui1_->Update();

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
    stageManager_->Draw();

    // 描画処理
   // if (!end) {
        player_->Draw();
        // Bulletマネージャの描画処理
        BulletManager::GetInstance()->Draw();
  //  }
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
        ui1_->Draw();
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
    float playerZ = player_->GetPosition().z;

    for (size_t tIndex = 0; tIndex < spawnTriggers_.size(); ++tIndex) {
        auto& trigger = spawnTriggers_[tIndex];

        if (!trigger.hasSpawned && playerZ >= trigger.zThreshold) {
        
            // --- フォーメーションの種類で分岐 ---
            if (tIndex == 0) {
                SpawnReverseStepFormation(trigger);

            }  if (tIndex == 1) {
                SpawnVFormation(trigger);
            
            }if (tIndex == 2) {                               
                SpawnZigZagFormation(trigger);

            }
            trigger.hasSpawned = true;
        }
    }
}
///====================================================
/// 敵出現パターン：ジグザグフォーメーション
///====================================================
void GamePlayScene::SpawnZigZagFormation(const EnemySpawnTrigger& trigger) {
    int activated = 0;
    const int numEnemies = 5;

    const float baseY = 5.0f;           // 中央Y
    const float baseZ = trigger.zThreshold + 50.0f;

    // X座標とY座標を個別指定（5体）
    const float xPositions[5] = { -1.5f, 1.5f, 0.0f, -1.5f, 1.5f };
    const float yPositions[5] = { 2.0f, 2.0f, 0.0f, -2.0f, -2.0f };

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive() && activated < numEnemies) {

            float posX = xPositions[activated];
            float posY = baseY + yPositions[activated];
            float posZ = baseZ;

            enemy->SetnewTranslate(Vector3{ posX, posY, posZ }, trigger.moveType);
            enemy->SetActive(true);

            ++activated;
        }
    }
}
///====================================================
/// 敵出現パターン：逆ステップフォーメーション
///====================================================
void GamePlayScene::SpawnReverseStepFormation(const EnemySpawnTrigger& trigger) {
    int activated = 0;

    const float baseX = 0.0f;     // 左右中央
    const float baseY = 5.0f;     // 一番上
    const float baseZ = trigger.zThreshold + 80.0f; // プレイヤーより少し前

    const float stepX = 1.5f;     // 横ズレ
    const float stepY = -1.2f;    // 縦ズレ
    const float stepZ = 5.0f;     // 奥行き方向

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive()) {
            // X方向のズレを逆に
            float posX = baseX - (stepX * activated); // ← 反転
            float posY = baseY + (stepY * activated);
            float posZ = baseZ - (stepZ * activated);

            // 敵をそのまま配置
            enemy->SetnewTranslate(Vector3{ posX, posY, posZ }, trigger.moveType);
            enemy->SetActive(true);

            ++activated;
            if (activated >= trigger.spawnCount) break;
        }
    }
}
///====================================================
/// 敵出現パターン：V字フォーメーション
///====================================================
void GamePlayScene::SpawnVFormation(const EnemySpawnTrigger& trigger) {
    int activated = 0;

    // 基準位置
    const float baseX = 0.0f;    // 中央
    const float baseY = 1.0f;    // 一番上
    const float stepX = 3.0f;    // 横の間隔
    const float stepY = -2.0f;   // 縦の間隔
    const float baseZ = trigger.zThreshold + 60.0f;

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive()) {
            float posX = baseX;
            float posY = baseY;

            // 出現位置をactivatedの順で決定
            switch (activated) {
            case 0: // 真ん中（頂点）
                posX = baseX;
                posY = baseY;
                break;
            case 1: // 左上
                posX = baseX - stepX;
                posY = baseY + stepY;
                break;
            case 2: // 右上
                posX = baseX + stepX;
                posY = baseY + stepY;
                break;
            case 3: // 左下
                posX = baseX - stepX * 2;
                posY = baseY + stepY * 2;
                break;
            case 4: // 右下
                posX = baseX + stepX * 2;
                posY = baseY + stepY * 2;
                break;
            default:
                // spawnCountが5を超えた場合の安全策
                posX = baseX + (activated - 2) * stepX;
                posY = baseY + stepY * 3;
                break;
            }

            enemy->SetnewTranslate(Vector3{ posX, posY, baseZ }, trigger.moveType);
            enemy->SetActive(true);

            ++activated;
            if (activated >= trigger.spawnCount) break;
        }
    }
}
///====================================================
/// OBB同士の当たり判定（分離軸定理）
/// ====================================================
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
///====================================================
/// プレイヤー弾 vs 敵の当たり判定
///====================================================
void GamePlayScene::CheckBulletEnemyCollisionsOBB() {
    //const auto& bullets = BulletManager::GetInstance()->GetPlayerBullets();

    //for (const std::unique_ptr<PlayerBullet>& bullet : bullets) {
    //    if (!bullet->IsActive()) continue;

    //    for (auto& enemy : enemies_) {
    //        if (!enemy->IsActive()) continue;

    //        OBB bulletOBB = bullet->GetOBB(); // bulletがOBB情報を持っている必要あり
    //        OBB enemyOBB = enemy->GetOBB();   // 敵のOBBも同様に

    //        if (IsOBBIntersect(bulletOBB, enemyOBB)) {
    //            bullet->SetInactive();
    //            enemy->SetInactive();
    //            // パーティクル生成など
    //            break;
    //        }
    //    }
    //}
}
///====================================================
/// 敵弾 vs プレイヤーの当たり判定
///====================================================
void GamePlayScene::CheckEnemyBulletPlayerCollisionsOBB() {
    //const auto& bullets = BulletManager::GetInstance()->GetEnemyBullets();

    //if (!player_ || !player_->IsActive()) return;

    //OBB playerOBB = player_->GetOBB(); // プレイヤーがOBBを返すようにしておく必要あり

    //for (const std::unique_ptr<EnemyBullet>& bullet : bullets) {
    //    if (!bullet->IsActive()) continue;

    //    OBB bulletOBB = bullet->GetOBB(); // 弾にもOBBが必要

    //    if (IsOBBIntersect(bulletOBB, playerOBB)) {
    //        bullet->SetInactive();
    //        player_->SetInactive();  // プレイヤーを無効にする

    //        end = true;
    //        // フェード開始             
    //        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
    //        // ヒットエフェクトなど追加
    //        break;
    //    }
    //}
}
