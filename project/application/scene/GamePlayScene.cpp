#include "GamePlayScene.h"
#include <SceneManager.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <SkyboxCommon.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
//#include <ParticleCommon.h>
//#include <ParticleManager.h>
#include <CameraManager.h>
#include <Input.h>
#include <StageManager.h>
#include <BulletManager.h>
#include <UIManager.h>
#include <FadeManager.h>
#include <GamePlayUI.h>
#include <SceneName.h>
#include <EventManager.h>
#include <CollisionManager.h>
#include <CameraDefs.h>
#include <PlayerState.h>
#include <EnemyListEditor.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace CameraDefs;
using namespace AssetGen;
using namespace AssetGen::LoadResourceID::Textures;

namespace MyGame {

    void GamePlayScene::Finalize() {
        BulletManager::GetInstance()->Finalize(); // 弾マネージャの終了処理
        StageManager::GetInstance()->Finalize();  // ステージマネージャの終了処理
        CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
        UIManager::GetInstance()->Finalize();     // UIマネージャの終了処理 
        FadeManager::GetInstance()->Finalize();   // フェードマネージャの終了処理
        CollisionManager::GetInstance()->Finalize(); // 衝突マネージャの終了処理
        EnemyListEditor::GetInstance()->Finalize();
    }

    void GamePlayScene::Initialize() {
        // カメラマネージャの初期化
        CameraManager::GetInstance()->Initialize(SceneName::GAMEPLAY);
        // プレイヤー生成,初期化
        player_ = std::make_unique<Player>();
        player_->Initialize();
        // GamePlayCameraにプレイヤーを渡す(プレイヤ―の位置を確認)
        CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()->SetPlayer(player_.get());

        // 敵生成
        const int kEnemyMax = 300;
        for (int i = 0; i < kEnemyMax; i++) {
            std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(); // 敵の生成
            enemy->Initialize(); // 敵の初期化
            enemy->SetActive(false); // 非アクティブ
            enemy->SetSpawned(false); // 出現フラグ
            enemy->SetPlayer(player_.get()); // プレイヤーへの参照をセット
            enemies_.push_back(std::move(enemy)); // 敵リストに追加
        }

        // 敵のSpawner生成,初期化
        enemySpawner_ = std::make_unique<EnemySpawner>();
        enemySpawner_->SetEnemies(&enemies_);    // 敵リストへの参照をセット
        enemySpawner_->SetPlayer(player_.get()); // プレイヤーへの参照をセット
        // imgui        
        EnemyListEditor::GetInstance()->SetEnemies(&enemies_);

        // 最初のスポーン
        player_->SetEnemy(enemies_.empty() ? nullptr : enemies_.front().get());

        // ステージマネージャの初期化
        StageManager::GetInstance()->Initialize();
        StageManager::GetInstance()->SetClearwallTranslate(CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()->GetRailEndPosition());

        // UIにプレイヤーの情報を渡す
        GamePlayUI* gameplayUI = UIManager::GetInstance()->GetUI<GamePlayUI>();
        if (gameplayUI) {
            gameplayUI->SetPlayer(player_.get());
        }
        // UIマネージャの初期化
        UIManager::GetInstance()->Initialize();
        // フェードマネージャの初期化(フェードイン開始処理)
        FadeManager::GetInstance()->StartFade(FadeType::FadeIn, FadeStyle::SilhouetteExplode, 1.0f);
        // ゲーム開始イベントの開始
        isGameStartEventDone_ = true;
#ifdef USE_IMGUI
        // 敵のパラメータ
        EnemyListEditor::GetInstance()->Initialize();
#endif // USE_IMGUI
    }

    void GamePlayScene::Update() {
        // カメラマネージャの更新
        CameraManager::GetInstance()->Update();
#pragma region 全てのObject3d個々の更新処理      
        // ポーズメニューのトリガーと更新
        if (!UIManager::GetInstance()->GetUI<GamePlayUI>()->GetPauseMenu()->IsActive() &&
            Input::GetInstance()->TriggerKey(DIK_TAB)) {
            UIManager::GetInstance()->GetUI<GamePlayUI>()->GetPauseMenu()->SetActive(true);
        }
        // ポーズがアクティブ中、ゲームの更新を停止してポーズメニューの更新のみ行う
        if (UIManager::GetInstance()->GetUI<GamePlayUI>()->GetPauseMenu()->IsActive()) {
            UIManager::GetInstance()->GetUI<GamePlayUI>()->GetPauseMenu()->Update();
            FadeManager::GetInstance()->Update();
            return;
        }

        ////  ゲーム開始前のイベント処理
        //if (!isGameStartEventDone_) {
        //    // ゲーム開始イベントの開始
        //    EventManager::GetInstance()->EventStart(Event::EventState::GameStart);
        //    isGameStartEventDone_ = true;
        //}

        // 敵スポーン
        enemySpawner_->Update();

        // 敵更新
        for (std::unique_ptr<Enemy>& enemy : enemies_) {
            enemy->Update();
        }

        // 死亡した敵の削除
        enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](std::unique_ptr<Enemy>& enemy) {
            if (!enemy->IsAlive()) {
                enemy->Finalize();
                return true;
            }
            return false;
            }),
            enemies_.end()
        );

        // カメラのターゲットとプレイヤーをセット（プレイヤーの位置にカメラを追従させるため）
        CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()->SetPlayer(player_.get());
        // プレイヤーの更新
        player_->Update();
        // 弾の更新
        BulletManager::GetInstance()->Update();
        // ステージマネージャの更新
        StageManager::GetInstance()->Update();
        // イベントマネージャの更新
        EventManager::GetInstance()->Update();
        // 全ての衝突をチェック 
        CollisionManager::GetInstance()->CheckAllCollisions();

        // ゲームクリアの条件をチェック
        if (CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()->GetFinished()) {
            // フェードアウト
            FadeManager::GetInstance()->SceneChangeFade(SceneName::GAMECLEAR, FadeStyle::SilhouetteExplode, 1.5f);
        }

        // パーティクル更新
   //     ParticleManager::GetInstance()->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理
        // UIマネージャの更新
        UIManager::GetInstance()->Update();
        // フェードマネージャの更新
        FadeManager::GetInstance()->Update();

#pragma endregion 全てのSprite個々の更新処理
    }

    void GamePlayScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
        // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
        SkyboxCommon::GetInstance()->Commondrawing();
        // ステージマネージャの描画
        StageManager::GetInstance()->DDSDraw();
        // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
        Object3dCommon::GetInstance()->Commondrawing();

        // プレイヤーの描画
        player_->Draw();

        // 敵の描画
        for (std::unique_ptr<Enemy>& enemy : enemies_) {
            enemy->Draw();
        }

        // 弾の描画
        BulletManager::GetInstance()->Draw();
        // ステージマネージャの描画
        StageManager::GetInstance()->Draw();
        // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
       // ParticleCommon::GetInstance()->Commondrawing();
       // ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理 
        // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
        SpriteCommon::GetInstance()->Commondrawing();
    
        // プレイヤーのスプライト描画
        player_->DrawSprite();

        // UIマネージャの描画
        UIManager::GetInstance()->Draw();
        // フェードマネージャの描画
        FadeManager::GetInstance()->Draw();
        // イベントマネージャの描画
        EventManager::GetInstance()->Draw2D();
#pragma endregion 全てのSprite個々の描画処理
    }
}