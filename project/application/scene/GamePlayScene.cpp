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

// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen;
using namespace AssetGen::LoadResourceID::Textures;

namespace MyGame {
    void GamePlayScene::Finalize() {}

    void GamePlayScene::Initialize() {
        // カメラマネージャの初期化
        CameraManager::GetInstance()->Initialize(Transform({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -50.0f }));


        TextureManager::GetInstance()->LoadTexture(Ui::Complete);
        TextureManager::GetInstance()->LoadTexture(Ui::Mission);

        Sprite_ = Sprite::Create(Ui::Complete, Vector2{ 50.0f, 100.0f }, 0.0f, Vector2{ 400.0f,150.0f });

        player_ = std::make_unique<Player>();
        player_->Initialize();
        CameraManager::GetInstance()->SetPlayer(player_.get());

        // ステージマネージャの初期化
        StageManager::GetInstance()->Initialize();
    }

    void GamePlayScene::Update() {
		// カメラマネージャの更新
        CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理


        player_->Update();

        // ステージマネージャの更新
        StageManager::GetInstance()->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理


        Sprite_->Update();


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


        player_->Draw();


        // ステージマネージャの描画
        StageManager::GetInstance()->Draw();
        // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
        ParticleCommon::GetInstance()->Commondrawing();
        //ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理 
        // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
        SpriteCommon::GetInstance()->Commondrawing();
        
        player_->DrawSprite();

        Sprite_->Draw();

#pragma endregion 全てのSprite個々の描画処理
    }
}