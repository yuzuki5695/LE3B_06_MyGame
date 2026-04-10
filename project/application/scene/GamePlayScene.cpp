#include "GamePlayScene.h"
#include <SceneManager.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <SkyboxCommon.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <CameraManager.h>
#include <Input.h>
#include <StageManager.h>
#include <BulletManager.h>
#include <UIManager.h>
#include <FadeManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>
#include <GamePlayUI.h>

using namespace MyEngine;
using namespace AssetGen;
using namespace AssetGen::LoadResourceID::Textures;

namespace MyGame {

    void GamePlayScene::Finalize() {
        BulletManager::GetInstance()->Finalize(); // 弾マネージャの終了処理
        StageManager::GetInstance()->Finalize();  // ステージマネージャの終了処理
        CameraManager::GetInstance()->Finalize(); // カメラマネージャの終了処理
		UIManager::GetInstance()->Finalize();     // UIマネージャの終了処理 
		FadeManager::GetInstance()->Finalize();   // フェードマネージャの終了処理
    }

    void GamePlayScene::Initialize() {
        // カメラマネージャの初期化
        CameraManager::GetInstance()->Initialize(Transform({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -10.0f }));

        player_ = std::make_unique<Player>();
        player_->Initialize();
        CameraManager::GetInstance()->SetPlayer(player_.get());


        // ステージマネージャの初期化
        StageManager::GetInstance()->Initialize();

        GamePlayUI* gameplayUI = UIManager::GetInstance()->GetUI<GamePlayUI>();
        if (gameplayUI) {
            gameplayUI->SetPlayer(player_.get());
        }
        // UIマネージャの初期化
        UIManager::GetInstance()->Initialize();
        FadeManager::GetInstance()->Initialize();
        FadeManager::GetInstance()->StartFade(FadeType::None, FadeStyle::SilhouetteExplode, 1.0f);
    }

    void GamePlayScene::Update() {
		// カメラマネージャの更新
        CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理

		// プレイヤーの更新
        player_->Update();             
		// 弾の更新
        BulletManager::GetInstance()->Update();
        // ステージマネージャの更新
        StageManager::GetInstance()->Update();
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

        // 弾の描画
        BulletManager::GetInstance()->Draw();
        // ステージマネージャの描画
        StageManager::GetInstance()->Draw();
        // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
        ParticleCommon::GetInstance()->Commondrawing();
        //ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理 
        // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
        SpriteCommon::GetInstance()->Commondrawing();
		// プレイヤーのスプライト描画
        player_->DrawSprite();
		// UIマネージャの描画
        UIManager::GetInstance()->Draw();
		// フェードの描画
        FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
    }
}