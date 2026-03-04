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
#include <Player.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include <Collision.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <Easing.h>

using namespace LoadResourceID;
using namespace Collision;
using namespace MatrixVector;
using namespace Easing;

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
    TextureManager::GetInstance()->LoadTexture(texture::Move);
    TextureManager::GetInstance()->LoadTexture(texture::Reticlemove);
    TextureManager::GetInstance()->LoadTexture(texture::Space);
    TextureManager::GetInstance()->LoadTexture(texture::Avoidance);
    TextureManager::GetInstance()->LoadTexture(texture::Gage);
    TextureManager::GetInstance()->LoadTexture(texture::PlayerUi);

    TextureManager::GetInstance()->LoadTexture(texture::W);
    TextureManager::GetInstance()->LoadTexture(texture::A);
    TextureManager::GetInstance()->LoadTexture(texture::S);
    TextureManager::GetInstance()->LoadTexture(texture::D);
    TextureManager::GetInstance()->LoadTexture(texture::W_RED);    
    TextureManager::GetInstance()->LoadTexture(texture::A_RED);    
    TextureManager::GetInstance()->LoadTexture(texture::S_RED);    
    TextureManager::GetInstance()->LoadTexture(texture::D_RED);    
    
    TextureManager::GetInstance()->LoadTexture(texture::ArrowUp);
    TextureManager::GetInstance()->LoadTexture(texture::ArrowLeft);
    TextureManager::GetInstance()->LoadTexture(texture::ArrowDown);
    TextureManager::GetInstance()->LoadTexture(texture::ArrowRight); 
    TextureManager::GetInstance()->LoadTexture(texture::ArrowUp_RED);
    TextureManager::GetInstance()->LoadTexture(texture::ArrowLeft_RED);
    TextureManager::GetInstance()->LoadTexture(texture::ArrowDown_RED);
    TextureManager::GetInstance()->LoadTexture(texture::ArrowRight_RED);

    TextureManager::GetInstance()->LoadTexture(texture::SPACEKey);    
    TextureManager::GetInstance()->LoadTexture(texture::SPACEKey_RED);
    TextureManager::GetInstance()->LoadTexture(texture::SHIFT);    
    TextureManager::GetInstance()->LoadTexture(texture::SHIFT_RED);

    Vector2 size = Vector2{ 40.0f,40.0f };
    Vector2 center = { 85.0f, 470.0f };
    CreateWASDUI(
        center, // WASD基準中心
        size,
        4.0f,   // キー間隔
        25.0f   // グループ間隔
    );
    isControlUIAnimating_ = false;
    controlUITimer_ = 0.0f;
	controlUIDuration_ = 0.5f; // アニメーションの総時間
	controlUIOriginalSize_ = size;
    uiOriginalSizes_.clear();
    for (auto& ui : uis_) {
        uiOriginalSizes_.push_back(ui->GetSize()); 
        ui->SetAnchorPoint({ 0.5f, 0.5f });
        ui->SetSize({ 0.0f, 0.0f }); // 全部0スタート
    };

    gage_ = Sprite::Create(texture::Gage, Vector2{ 380.0f, 10.0f }, 0.0f, Vector2{ 500.0f,30.0f });
    gage_->SetTextureSize(Vector2{ 500.0f,30.0f });
    player_ui_ = Sprite::Create(texture::PlayerUi, Vector2{ 380.0f, 12.3f }, 0.0f, Vector2{ 25.0f,25.0f });
    player_ui_->SetTextureSize(Vector2{ 25.0f,25.0f });

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
    // ゲームカメラの移動許可
   // CameraManager::GetInstance()->GetGameCamera()->Setmovefige(true);
    // ステージマネージャの初期化
    StageManager::GetInstance()->Initialize();

    end = false;
    // ポーズメニューの初期化
    pausemenu_ = std::make_unique<Pausemenu>();
    pausemenu_->Initialize();
    isPaused_ = false;
    isPausedevent_ = false;
}
///====================================================
/// 毎フレーム更新処理
///====================================================
void GamePlayScene::Update() { 
    pausemenu_->IconUpdate();
    // Enterキーでポーズの「開始」のみをチェック
    if (isPausedevent_ && !isPaused_ && Input::GetInstance()->Triggrkey(DIK_TAB)) {
        isPaused_ = true;
        pausemenu_->SetActive(true); // 演出開始！
    }

    //  ポーズ中の処理
    if (isPaused_) {
        pausemenu_->Update();
        // ポーズメニュー内での演出（逆再生含む）がすべて終わったかチェック
        if (pausemenu_->IsFinished()) {
            isPaused_ = false; // ゲーム再開
        }

        // Resume（再開）の場合は、メニューが閉じ終わるのを待つ
        if (pausemenu_->IsFinished() && pausemenu_->GetCommand() == PauseCommand::Resume) {
            isPaused_ = false;
        } else if (pausemenu_->GetCommand() == PauseCommand::GoToTitle) {
            // フェードマネージャの更新   
            FadeManager::GetInstance()->Update();
            // メニューが表示されたまま、フェードアウトを開始
            if (!goal_) {
                FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
                goal_ = true; // フェード開始フラグとして利用
            }

            // フェードが終わったらシーン遷移
            if (FadeManager::GetInstance()->IsFadeEnd()) {
                SceneManager::GetInstance()->ChangeScene("TITLE");
            }
            // タイトル移行時はここでreturnし、背後のゲーム処理を止めたままにする
            return;
        }
        // ポーズ演出中はゲームの他の更新を止める
        return;
    }

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
    if (player_->GetState() == State::Dead && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        CameraManager::GetInstance()->SetMode(CameraMode::Default);
        CameraManager::GetInstance()->SetTypeview(ViewCameraType::Sub);
        // フェード開始             
        end = false;
    }

    if (CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Sub && CameraManager::GetInstance()->GetMode() == CameraMode::Default) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        CameraManager::GetInstance()->SetMode(CameraMode::Follow);
    }

    // ゴール演出
    if (!goal_ && player_->GetState() == State::Alive && player_->GetPosition().z >= CameraManager::GetInstance()->GetGameplayCamera()->GetBezierPoints().back().controlPoint.z && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
        player_->SetState(State::Goal);
        // フェード開始             
        goal_ = true;
        end = false;
    }

    // ゲームスタートイベントが終了したらプレイヤ―操作可能に
    if (player_->GetState() == State::None && EventManager::GetInstance()->IsFinished() && CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        // イベント終了 → プレイヤーを操作可能に
        player_->SetState(State::Alive);
        isPausedevent_ = true;
        // 進行度を設定
        StartStageProgressUI();
    }
    
    if (EventManager::GetInstance()->GetProgress() >= 2.5f) {
        // 操作UIのアニメーション開始
        isControlUIAnimating_ = true;
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
    // 終了しない限り更新処理
    if (!end) {
        if (EventManager::GetInstance()->IsFinished() ){
            // 敵出現動作
            enemySpawner_->Update();
        }
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

    // 死んだ敵の削除
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& e) {
                return e->IsDead();  // ← 出現前の非アクティブは残す！
            }),
        enemies_.end());
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
    
    UpdateControlUIAnimation();
    UpdateControlUI();
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

    if (CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
        for (std::unique_ptr<Sprite>& ui : uis_) {
            ui->Draw();
        }
    }
    
    gage_->Draw();
    player_ui_->Draw();
    if (isPausedevent_) {
        if (isPaused_) {
            pausemenu_->Draw();
        }
        pausemenu_->IconDraw();
    }
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
    // プレイヤーのOBBを取得
    OBB playerOBB = player_->GetOBB();

    for (auto& enemy : enemies_) {
        if (!enemy->IsActive() || enemy->IsDead()) continue;

        // 敵のOBBを取得
        OBB enemyOBB = enemy->GetOBB();

        // 衝突判定
        if (IsOBBIntersect(playerOBB, enemyOBB)) {
            player_->SetState(State::Dead);
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

void GamePlayScene::CreateWASDUI(
    const Vector2& baseCenter,     // WASD中心
    const Vector2& size,
    float keySpacing,           // キー同士の隙間
    float groupSpacing)         // グループ間の縦間隔
{
    uis_.clear();

    float groupHeight = size.y * 2 + keySpacing;
    // W段 + ASD段の高さ

    // =====================
    // 1. WASD グループ
    // =====================
    Vector2 wasdCenter = baseCenter;

    Vector2 wPos = { wasdCenter.x, wasdCenter.y };
    Vector2 sRowY = { 0.0f,wasdCenter.y + size.y + keySpacing };

    Vector2 aPos = { wasdCenter.x - size.x - keySpacing, sRowY.y };
    Vector2 sPos = { wasdCenter.x, sRowY.y };
    Vector2 dPos = { wasdCenter.x + size.x + keySpacing, sRowY.y };

    uis_.push_back(Sprite::Create(texture::W, wPos, 0.0f, size)); //0
    uis_.push_back(Sprite::Create(texture::A, aPos, 0.0f, size)); //1
    uis_.push_back(Sprite::Create(texture::S, sPos, 0.0f, size)); //2
    uis_.push_back(Sprite::Create(texture::D, dPos, 0.0f, size)); //3


    // =====================
    // 2. Arrow グループ
    // =====================
    float arrowTopY = wasdCenter.y + groupHeight + groupSpacing;

    Vector2 arrowTop = { baseCenter.x, arrowTopY };
    float arrowRowY = arrowTopY + size.y + keySpacing;

    Vector2 upPos = { arrowTop.x, arrowTop.y };
    Vector2 leftPos = { arrowTop.x - size.x - keySpacing, arrowRowY };
    Vector2 downPos = { arrowTop.x, arrowRowY };
    Vector2 rightPos = { arrowTop.x + size.x + keySpacing, arrowRowY };

    uis_.push_back(Sprite::Create(texture::ArrowUp, upPos, 0.0f, size));     //4
    uis_.push_back(Sprite::Create(texture::ArrowRight, leftPos, 0.0f, size)); //5
    uis_.push_back(Sprite::Create(texture::ArrowDown, downPos, 0.0f, size)); //6
    uis_.push_back(Sprite::Create(texture::ArrowLeft, rightPos, 0.0f, size));//7


    // =====================
    // 3. SPACE
    // =====================
    Vector2 spaceSize = {80.0f, 40.0f};
    float spaceY = arrowTopY + groupHeight + groupSpacing;
    // 1つ目（中央）
    Vector2 spacePos = { baseCenter.x - (spaceSize.x + keySpacing) * 0.5f, spaceY };

    // 2つ目（右側）
    Vector2 spacePos2 = { baseCenter.x + (spaceSize.x + keySpacing) * 0.5f, spaceY };

    uis_.push_back(Sprite::Create(texture::SPACEKey, spacePos, 0.0f, spaceSize));  //8
    uis_.push_back(Sprite::Create(texture::SPACEKey, spacePos2, 0.0f, spaceSize)); //9
}

void GamePlayScene::UpdateControlUI() {
    Input* input = Input::GetInstance();
    if (EventManager::GetInstance()->IsFinished()){
        // === WASD ===
        uis_[0]->SetTexture(input->Pushkey(DIK_W) ? texture::W_RED : texture::W);
        uis_[1]->SetTexture(input->Pushkey(DIK_A) ? texture::A_RED : texture::A);
        uis_[2]->SetTexture(input->Pushkey(DIK_S) ? texture::S_RED : texture::S);
        uis_[3]->SetTexture(input->Pushkey(DIK_D) ? texture::D_RED : texture::D);
        // === Arrow ===
        uis_[4]->SetTexture(input->Pushkey(DIK_UP) ? texture::ArrowUp_RED : texture::ArrowUp);
        uis_[5]->SetTexture(input->Pushkey(DIK_RIGHT) ? texture::ArrowRight_RED : texture::ArrowRight);
        uis_[6]->SetTexture(input->Pushkey(DIK_DOWN) ? texture::ArrowDown_RED : texture::ArrowDown);
        uis_[7]->SetTexture(input->Pushkey(DIK_LEFT) ? texture::ArrowLeft_RED : texture::ArrowLeft);
        // === SPACE ===
        uis_[8]->SetTexture(input->Pushkey(DIK_SPACE) ? texture::SPACEKey_RED : texture::SPACEKey);
        uis_[9]->SetTexture(input->Pushkey(DIK_LSHIFT) ? texture::SHIFT_RED : texture::SHIFT);
    }
}

void GamePlayScene::UpdateControlUIAnimation() {
    if (!isControlUIAnimating_) return;

    controlUITimer_ += 1.0f / 60.0f;
    float t = controlUITimer_ / controlUIDuration_;

    if (t >= 1.0f) {
        t = 1.0f;
        isControlUIAnimating_ = false;
    }

    float ease = Easing::EaseOutBack(t);

    for (uint32_t i = 0; i < uis_.size(); ++i) {
        float width  = Easing::Lerp(0.0f, uiOriginalSizes_[i].x, ease);
        float height = Easing::Lerp(0.0f, uiOriginalSizes_[i].y, ease);

        uis_[i]->SetSize({ width, height });
    }
}