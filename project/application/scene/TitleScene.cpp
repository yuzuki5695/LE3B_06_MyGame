#include "TitleScene.h"
#include<SceneManager.h>
#include<TextureManager.h>
#include<ModelManager.h>
#include<SpriteCommon.h>
#include<Object3dCommon.h>
#include <CameraManager.h>
#include<Input.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include <ParticleCommon.h>
#include<SkyboxCommon.h>

void TitleScene::Finalize() {}

void TitleScene::Initialize() {
#pragma region 最初のシーンの初期化  
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));

    TextureManager::GetInstance()->LoadTexture("Title/newstart.png");
    TextureManager::GetInstance()->LoadTexture("Title/Enter.png");
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel("Title/Title.obj");
    ModelManager::GetInstance()->LoadModel("Tile.obj");
    ModelManager::GetInstance()->LoadModel("Player.obj");

    title_ = Object3d::Create("Title/Title.obj", { {0.7f,0.7f,0.7f},{0.0f,0.0f,0.0f},{0.0f,1.0f,10.0f} });
    Box_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });

    ui1_ = Sprite::Create("Title/newstart.png", Vector2{ 420.0f, 500.0f }, 0.0f, Vector2{ 180.0f,90.0f });
    ui1_->SetTextureSize(Vector2{ 180.0f,90.0f });
    ui2_ = Sprite::Create("Title/Enter.png", Vector2{ 580.0f, 500.0f }, 0.0f, Vector2{ 180.0f,90.0f });
    ui2_->SetTextureSize(Vector2{ 180.0f,90.0f });

    tile_ = Object3d::Create("Tile.obj", { { 20.0f, 1.0f, 300.0f }, { 0.0f, 0.9f, 0.0f }, { 0.0f, -8.0f, 63.4f } });
  
    playertransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.9f, 0.0f},  -10.0f,0.0f,40.0f };
    player_ = Object3d::Create("Player.obj", playertransform_);

#pragma endregion 最初のシーンの初期化
}

void TitleScene::Update() {
    // ENTERキーを押したら
    if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
        // シーン切り替え
        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();
#pragma region 全てのObject3d個々の更新処理
    Box_->Update();

    
    static float time = 0.0f;          // 経過時間
    time += 0.03f;                     // 更新速度（0.05f は揺れの速さ）

    Transform t = player_->GetTransform();
    t.translate.y = 0.0f + sinf(time) * 2.3f;  // 中心Y=-10.0f、振幅3.0f
    player_->SetTranslate(t.translate);

    title_->Update();
    tile_->Update(); 
    player_->Update();


#pragma endregion 全てのObject3d個々の更新処理

    ui1_->Update();
    ui2_->Update();

#pragma region 全てのSprite個々の更新処理

#pragma endregion 全てのSprite個々の更新処理
#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI
    // ImGuiでTileの位置・回転・スケールを変更
    ImGui::Begin("Tile Controller");

   // Transform t = tile_->GetTransform();

    ImGui::Text("Position");
    ImGui::DragFloat3("Translation", &playertransform_.translate.x, 0.1f);

    ImGui::Text("Rotation");
    ImGui::DragFloat3("Rotation", &playertransform_.rotate.x, 0.1f);

    
    player_->SetTranslate(playertransform_.translate);
    player_->SetRotate(playertransform_.rotate);

    ImGui::End();
#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了

}

void TitleScene::Draw() {
#pragma region 全てのObject3d個々の描画処理 
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    Box_->Draw();

    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();

    title_->Draw();
	tile_->Draw();
    player_->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();

    ui1_->Draw();
    ui2_->Draw();

#pragma endregion 全てのSprite個々の描画処理
}