#include "GamePlayScene.h"
#include <SceneManager.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <CameraManager.h>
#include <Input.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen;
using namespace AssetGen::LoadResourceID::Models;

void GamePlayScene::Finalize() {
}

void GamePlayScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 0.0f }));


    TextureManager::GetInstance()->LoadTexture("Textures/Ui/Complete.png");
    Sprite_ = Sprite::Create("Textures/Ui/Complete.png", Vector2{ 50.0f, 100.0f }, 0.0f, Vector2{ 400.0f,150.0f });

   //ModelManager::GetInstance()->LoadModel("Models/Character/Player/Player.obj");
    ModelManager::GetInstance()->LoadModel(Character::Player);
    player_ = Object3d::Create(Character::Player, { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,10.0f} });

}

void GamePlayScene::Update() {       
    CameraManager::GetInstance()->Update();

    if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
    

    }

#pragma region 全てのObject3d個々の更新処理
   

    player_->Update();


#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理
    Sprite_->Update();
#pragma endregion 全てのSprite個々の更新処理

#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI 

#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了 
}

void GamePlayScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
  //  SkyboxCommon::GetInstance()->Commondrawing();
 //   Box_->Draw();
    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();
   
    
    player_->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
 ///   ParticleCommon::GetInstance()->Commondrawing();
    //ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();

    Sprite_->Draw();

#pragma endregion 全てのSprite個々の描画処理
}

