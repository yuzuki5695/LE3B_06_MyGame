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
#include<CharacterManager.h>

void GamePlayScene::Finalize() {}

void GamePlayScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 10.0f, -30.0f }, { 0.3f, 0.0f, 0.0f }));

    // テクスチャを読み込む
    TextureManager::GetInstance()->LoadTexture("uvChecker.png");
    TextureManager::GetInstance()->LoadTexture("monsterBall.png");
    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel("terrain.obj"); 
    ModelManager::GetInstance()->LoadModel("monsterBallUV.obj");

    // オブジェクトの作成
    grass = Object3d::Create("terrain.obj", Transform({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} })); 



    ModelManager::GetInstance()->LoadModel("Bullet/PlayerBullet.obj"); 
    ModelManager::GetInstance()->LoadModel("Tile.obj");

}

void GamePlayScene::Update() {
    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();
 
    // ENTERキーを押したら
    if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
        // シーン切り替え
        //SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
    }

#pragma region 全てのObject3d個々の更新処理

    // 更新処理 
    grass->Update();

    ParticleManager::GetInstance()->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理


#pragma endregion 全てのSprite個々の更新処理
    
#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI
    Object3dCommon::GetInstance()->DrawImGui();

    // Camera
    CameraManager::GetInstance()->DrawImGui();

#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了 
}

void GamePlayScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();
    
    // 描画処理
    grass->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
    ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();


#pragma endregion 全てのSprite個々の描画処理
}