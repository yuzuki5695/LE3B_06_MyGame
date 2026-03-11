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
#include <UIManager.h>
#include <GamePlayUI.h>

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
}
void GamePlayScene::PauseMenuUpdate() {
    //  Enterキーでポーズの「開始」のみをチェック
    if (isPausedevent_ && !pausemenu_->IsActive() && Input::GetInstance()->Triggrkey(DIK_TAB)) {
        pausemenu_->SetActive(true); // 演出開始！
    }
    pausemenu_->Update();
}

void GamePlayScene::Update() {  
	// ポーズメニューの更新
    PauseMenuUpdate();
    //  ポーズ中の処理
    if (pausemenu_->IsActive()) {

        if (pausemenu_->GetCommand() == PauseCommand::GoToTitle) {
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
        return; // ポーズ中は他の更新をスキップ
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
        GamePlayUI* gpUI = UIManager::GetInstance()->GetUI<GamePlayUI>();
        // 見つかった場合のみ固有のメソッドを呼ぶ
        if (gpUI) {        
            // 進行度を設定        
            gpUI->StartStageProgressUI();
        }
    }
    
    if (EventManager::GetInstance()->GetProgress() >= 2.5f) {
        // 操作UIのアニメーション開始
        GamePlayUI* gpUI = UIManager::GetInstance()->GetUI<GamePlayUI>();
        // 見つかった場合のみ固有のメソッドを呼ぶ
        if (gpUI) {
            gpUI->StartControlUIAnimation();
        }
    }

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
        // 更新処理
        player_->Update();
        // プレイヤーがゴール手前までは敵も更新
        if (player_->GetPosition().z <=  CameraManager::GetInstance()->GetGameplayCamera()->GetBezierPoints().back().controlPoint.z) {

        }
        wall->Update();
        // Bulletマネージャの更新処理
        BulletManager::GetInstance()->Update();
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

    // 描画処理
    if (!end) {
        player_->Draw();
        // Bulletマネージャの描画処理
        BulletManager::GetInstance()->Draw();
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