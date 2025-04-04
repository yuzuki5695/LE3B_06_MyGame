#include "GamePlayScene.h"
#include<TextureManager.h>
#include<ModelManager.h>
#include<SpriteCommon.h>
#include<Object3dCommon.h>
#include<Input.h>
#include<ImGuiManager.h>
#include<SceneManager.h>
#include"ParticleManager.h"

void GamePlayScene::Finalize() {
    // パーティクルマネージャの開放
    ParticleManager::GetInstance()->Finalize();
}

void GamePlayScene::Initialize() {
    // テクスチャを読み込む
    TextureManager::GetInstance()->LoadTexture("Resources/uvChecker.png");
    TextureManager::GetInstance()->LoadTexture("Resources/monsterBall.png");
    TextureManager::GetInstance()->LoadTexture("Resources/circle.png");
    // 変数に代入
    TexturePath01 = "Resources/uvChecker.png";
    TexturePath02 = "Resources/monsterBall.png";
    TexturePath03 = "Resources/circle.png";

    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel("plane.obj");
    ModelManager::GetInstance()->LoadModel("axis.obj");
    // 変数に代入
    ModelPath01 = "plane.obj";

#pragma region 最初のシーンの初期化

    // スプライトの初期化
    sprite = Sprite::Create(TexturePath01, { 100.0f,100.0f }, 0.0f, { 360.0f,360.0f });

    // 3Dモデルの初期化
    model = std::make_unique <Model>();
    model->Initialize(ModelManager::GetInstance()->GetModelCommon(), "Resources", ModelPath01);

    // object3dの初期化
    object = Object3d::Create(ModelPath01, { { 1.0f, 1.0f, 1.0f }, { 0.0f, 3.0f, 0.0f }, { 0.0f, -0.5f, 0.0f } });

    // パーティクル
    // モデル生成
    ParticleManager::GetInstance()->SetParticleModel("Resources", ModelPath01);
    // テクスチャ生成
    ParticleManager::GetInstance()->CreateParticleGroup("Particles", TexturePath01);
    ParticleManager::GetInstance()->CreateParticleGroup("uvChecker", TexturePath01);
    // 発生
    ParticleManager::GetInstance()->Emit("Particles", Vector3{ 0.0f, -0.5f, 0.0f }, 1);

#pragma endregion 最初のシーンの初期化
}

void GamePlayScene::Update() {
#pragma region  ImGuiの更新処理開始
    // デモウィンドウの表示
    //ImGui::ShowDemoWindow();
    // スプライト
    //sprite->DebugUpdata();
    // object3d
    //object_->DebugUpdata();
    // パーティクル
    ParticleManager::GetInstance()->DebugUpdata();

#pragma endregion ImGuiの更新処理終了  

    /*-------------------------------------------------------------------------------------------------------*/
    /*-----------------------------------3Dオブジェクトの更新処理の開始------------------------------------------*/
    /*------------------------------------------------------------------------------------------------------*/

    // パーティクルの更新処理
    ParticleManager::GetInstance()->Update();

    object->Update();

    /*-------------------------------------------------------------------------------------------------------*/
    /*-----------------------------------3Dオブジェクトの更新処理の終了------------------------------------------*/
    /*------------------------------------------------------------------------------------------------------*/

    /*----------------------------------------------------------------------------------------------------*/
    /*---------------------------------------Spriteの更新処理----------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------*/

    // 更新処理
    sprite->Update();

    /*----------------------------------------------------------------------------------------------------*/
    /*-------------------------------------Spriteの更新処理終了----------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------*/
    // ImGuiの描画前準備
    ImGuiManager::GetInstance()->End();
}

void GamePlayScene::Draw() {
    /*------------------------------------------------------------------------------------------------------*/
    /*----------------------------------3Dオブジェクトの描画処理開始--------------------------------------------*/
    /*-----------------------------------------------------------------------------------------------------*/
    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();


    object->Draw();

    /*------------------------------------------------------------------------------------------------------*/
    /*----------------------------------3Dオブジェクトの描画処理終了--------------------------------------------*/
    /*-----------------------------------------------------------------------------------------------------*/

    /*----------------------------------------------------------------------------------------------------*/
    /*------------------------------------Spriteの描画処理開始----------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------*/
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();


    //// Spriteの描画は常にuvCheckerにする
    //dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

    sprite->Draw();

    /*----------------------------------------------------------------------------------------------------*/
    /*------------------------------------Spriteの描画処理終了----------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------*/
    
    /*-------------------------------------------------------------------------------------------------------*/
    /*-------------------------------------パーティクルの更新処理の開始------------------------------------------*/
    /*------------------------------------------------------------------------------------------------------*/
    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む
    ParticleManager::GetInstance()->Commondrawing();


    // パーティクルの描画処理  
    ParticleManager::GetInstance()->Draw();
 
    /*-------------------------------------------------------------------------------------------------------*/
    /*-------------------------------------パーティクルの更新処理の開始------------------------------------------*/
    /*------------------------------------------------------------------------------------------------------*/
}