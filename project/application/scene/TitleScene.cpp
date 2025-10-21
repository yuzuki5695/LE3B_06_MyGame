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
#include<FadeManager.h>

void TitleScene::Finalize() {
    FadeManager::GetInstance()->Finalize();
}

void TitleScene::Initialize() {
#pragma region 最初のシーンの初期化  
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));

    TextureManager::GetInstance()->LoadTexture("Title/UI_02.png");
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
    // .objファイルからモデルを読み込む
    ModelManager::GetInstance()->LoadModel("Title/Title.obj");
    ModelManager::GetInstance()->LoadModel("Tile.obj");
    ModelManager::GetInstance()->LoadModel("Player.obj");

 
    Box_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });

    ui1_ = Sprite::Create("Title/UI_02.png", Vector2{ 420.0f, 500.0f }, 0.0f, Vector2{ 180.0f,90.0f });
    ui1_->SetTextureSize(Vector2{ 180.0f,90.0f });
    ui2_ = Sprite::Create("Title/UI_02.png", Vector2{ 420.0f, 500.0f }, 0.0f, Vector2{ 360.0f,90.0f });
    ui2_->SetTextureSize(Vector2{ 360.0f,90.0f });


    playertransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.9f, 0.0f},  -20.0f,0.0f,40.0f };
    player_ = Object3d::Create("Player.obj", playertransform_);
    startX = -20.0f;
    endX = -10.0f;

    timer = 0.0f;
    moveDuration = 240.0f;  // 移動にかけるフレーム数（約2秒）     
    moveFinished = false;
    time = 0.0f;          // 経過時間 

    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();

    ui3_ = Sprite::Create("Title/Title.png", Vector2{ 300.0f, 100.0f }, 0.0f, Vector2{ 600.0f,300.0f });
    ui3_->SetTextureSize(Vector2{ 600.0f,300.0f });

#pragma endregion 最初のシーンの初期化
}

void TitleScene::Update() {
    // フェードイン開始
    if (!FadeManager::GetInstance()->IsFadeStart() && !FadeManager::GetInstance()->IsFading()) {
        // フェード開始
        FadeManager::GetInstance()->StartFadeIn(1.0f, FadeStyle::SilhouetteSlide);
    }
    // フェードマネージャの更新処理
    FadeManager::GetInstance()->Update();
       // ゲームシーンへの入力処理
    if (Input::GetInstance()->Triggrkey(DIK_RETURN) && !FadeManager::GetInstance()->IsFading() && FadeManager::GetInstance()->IsFadeEnd()) {
        // フェード開始
        FadeManager::GetInstance()->StartFadeOut(1.0f,FadeStyle::SilhouetteExplode);
    }

    // フェードアウトが完了したら次のシーンへ
    if (FadeManager::GetInstance()->IsFadeEnd() && FadeManager::GetInstance()->GetFadeType() == FadeType::FadeOut) {
        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();
#pragma region 全てのObject3d個々の更新処理 
    // Skyboxの回転
    Transform skyTrans = Box_->GetTransform();
    skyTrans.rotate.y += 0.001f; // Y軸回転（1フレームごとに少しずつ）
    Box_->SetRotate(skyTrans.rotate);
    Box_->Update();

    if (!moveFinished) {
        // --- 位置のイージング ---
        timer += 1.0f;
        float t = std::clamp(timer / moveDuration, 0.0f, 1.0f);

        // EaseOutCubic
        float easeT = 1.0f - powf(1.0f - t, 3.0f);

        float startX = -20.0f;
        float endX   = -10.0f;

        playertransform_.translate.x = startX + (endX - startX) * easeT;

        // 移動が完了したらフラグを立てる
        if (t >= 1.0f) {
            moveFinished = true;
            timer = 0.0f;  // カラー用にリセット
        }

        player_->SetTranslate(playertransform_.translate);
    }


    time += 0.03f;                     // 更新速度（0.05f は揺れの速さ） 
    playertransform_.translate.y = 0.0f + sinf(time) * 2.3f;  // 中心Y=-10.0f、振幅3.0f 
    player_->SetTranslate(playertransform_.translate);

    //title_->Update();
    player_->Update();


#pragma endregion 全てのObject3d個々の更新処理
	ui3_->Update();
    ui1_->Update();
    ui2_->Update();

#pragma region 全てのSprite個々の更新処理

#pragma endregion 全てのSprite個々の更新処理
#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI
    // ImGuiでTileの位置・回転・スケールを変更
    ImGui::Begin("Tile Controller");

    ImGui::Text("Position");
    ImGui::DragFloat3("Translation", &playertransform_.translate.x, 0.1f);

    ImGui::Text("Rotation");
    ImGui::DragFloat3("Rotation", &playertransform_.rotate.x, 0.1f);

    
    player_->SetTranslate(playertransform_.translate);
    player_->SetRotate(playertransform_.rotate);

    ImGui::End();

	FadeManager::GetInstance()->DrawImGui(); // フェードマネージャのImGui制御

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

    player_->Draw();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();
	
    
    ui3_->Draw();
    //ui1_->Draw();
    ui2_->Draw();

	// フェードの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}