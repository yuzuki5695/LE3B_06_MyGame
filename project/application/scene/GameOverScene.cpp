#include "GameOverScene.h"
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
#include <random>
///====================================================
/// 終了処理
///====================================================
void GameOverScene::Finalize() {
    FadeManager::GetInstance()->Finalize();
}
///====================================================
///  初期化処理
///====================================================
void GameOverScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    CameraManager::GetInstance()->SetCameraMode(CameraMode::Default);
    // モデルの読み込み    
    ModelManager::GetInstance()->LoadModel("Gameover/Model/parts_01.obj");
    ModelManager::GetInstance()->LoadModel("Gameover/Model/parts_02.obj");
    ModelManager::GetInstance()->LoadModel("Gameover/Model/parts_03.obj");
    ModelManager::GetInstance()->LoadModel("Gameover/Model/parts_04.obj");

    // テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture("TitleEnter.png");
    TextureManager::GetInstance()->LoadTexture("CubemapBox.dds");
    TextureManager::GetInstance()->LoadTexture("Gameover/Texture/Gameover.png");
    TextureManager::GetInstance()->LoadTexture("Gameover/Texture/UI_01.png");

    ui1Timer_ = 0.0f;
    ui1Duration_ = 60.0f; // 約1秒
    ui2Timer_ = 0.0f;
    ui2Duration_ = 60.0f;
    ui1StartPos = { 1000.0f, -100.0f }; // 上にずらした初期位置
    ui1EndPos = { 1000.0f, 200.0f }; // 最終位置（現在の値）
    ui2StartPos = { 1000.0f, -250.0f };
    ui2EndPos = { 1000.0f, 350.0f };
    phase_ = 0;

    ui1_ = Sprite::Create("Gameover/Texture/Gameover.png", ui1StartPos, 0.0f, Vector2{ 300.0f,120.0f });
    ui1_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準
    ui1_->SetTextureSize(Vector2{ 300.0f,120.0f });
    ui2_ = Sprite::Create("Gameover/Texture/UI_01.png", ui2StartPos, 0.0f, Vector2{ 360.0f,100.0f });
    ui2_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準
    ui2_->SetTextureSize(Vector2{ 360.0f,100.0f });

    Box_ = Skybox::Create("CubemapBox.dds", Transform{ { 1000.0f, 1000.0f, 1000.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f } });

    // ランダムXを生成（-0.5～0.5の範囲）
    std::random_device rd;
    std::mt19937 randomEngine(rd());
    std::uniform_real_distribution<float> transformY(10.0f, 13.0f);
    std::uniform_real_distribution<float> distX(-10.0f, 3.0f);
    std::uniform_real_distribution<float> distY(0.01f, 0.03f);
    std::uniform_real_distribution<float> rotateX(-0.01f, 0.01f);
    std::uniform_real_distribution<float> rotateY(-0.01f, 0.01f);
    std::uniform_real_distribution<float> rotateZ(-0.01f, 0.01f);

    std::vector<std::string> textureNames = {
    "Gameover/Model/parts_01.obj",
    "Gameover/Model/parts_02.obj",
    "Gameover/Model/parts_03.obj",
    "Gameover/Model/parts_04.obj",
    };

    partCount_ = 7;
    for (size_t  i = 0; i < partCount_; ++i) {
        PartInfo part;

        part.transform = { {1.0f, 1.0f, 1.0f}, {0.0f,0.0f,0.0f}, {distX(randomEngine), transformY(randomEngine), 30.0f} };
        // 1つ目はそのまま、それ以降は2個ずつテクスチャを切り替え
        size_t textureIndex = 0; // 1つ目は 0
        if (i > 0) {
            textureIndex = ((i - 1) / 2) + 1; // 2個ごとに切り替え
            if (textureIndex >= textureNames.size()) textureIndex = textureNames.size() - 1; // 配列オーバー防止
        }

        part.obj = Object3d::Create("Gameover/Model/parts_01.obj", part.transform);
        part.obj->SetModel(textureNames[textureIndex]); // 後からテクスチャを設定

        part.fallSpeed = { 0.0f, distY(randomEngine), 0.0f }; // Y軸落下
        part.rotateSpeed = { rotateX(randomEngine), rotateY(randomEngine), rotateZ(randomEngine) };

        partsList.push_back(std::move(part));
    }

    // フェードマネージャの初期化
    FadeManager::GetInstance()->Initialize();
}
///====================================================
/// 更新処理
///====================================================
void GameOverScene::Update() {
    // フェードイン開始
    if (!FadeManager::GetInstance()->IsFadeStart() && !FadeManager::GetInstance()->IsFading()) {
        // フェード開始
        FadeManager::GetInstance()->StartFadeIn(1.0f, FadeStyle::Normal);
    }
    // フェードマネージャの更新処理
    FadeManager::GetInstance()->Update();

    // ゲームシーンへの入力処理
    if (Input::GetInstance()->Triggrkey(DIK_RETURN) && !FadeManager::GetInstance()->IsFading() && FadeManager::GetInstance()->IsFadeEnd()) {
        // UI演出完了後のみキー入力を許可
        if (phase_ == 2) {
            // フェード開始
            FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::SilhouetteSlide);
        }
    }

    // フェードアウト完了後にタイトルへ
    if (FadeManager::GetInstance()->IsFadeEnd() && FadeManager::GetInstance()->GetFadeType() == FadeType::FadeOut) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }

    /*-------------------------------------------*/
    /*--------------Cameraの更新処理---------------*/
    /*------------------------------------------*/
    CameraManager::GetInstance()->Update();

#pragma region 全てのObject3d個々の更新処理
    Box_->Update();   // 背景の更新
   
    UpdateParts(); //  落下処理

#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理
    

    UpdateUI(); // UIアニメーション

   
#pragma endregion 全てのSprite個々の更新処理
#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI

    // Camera
    CameraManager::GetInstance()->DrawImGui();


#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了

}
///====================================================
/// 描画処理
///====================================================
void GameOverScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    SkyboxCommon::GetInstance()->Commondrawing();
    Box_->Draw(); // 背景の描画

    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();
    
    for (PartInfo& part : partsList) {
        part.obj->Draw();
    }

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
    ParticleCommon::GetInstance()->Commondrawing();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();

    ui1_->Draw(); // スプライトの描画
    ui2_->Draw();

    // フェードマネージャの描画
    FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}

void GameOverScene::UpdateParts() {
    for (PartInfo& part : partsList) {
        if (!FadeManager::GetInstance()->IsFading() && FadeManager::GetInstance()->IsFadeEnd() || phase_ == 2) {
            // 落下
            part.transform.translate.y -= part.fallSpeed.y;
            if (part.transform.translate.y < -10.0f) {
                part.transform.translate.y = -10.0f; // 地面で止める
            }

            // 回転
            part.transform.rotate -= part.rotateSpeed;

            // 更新反映
            part.obj->SetTranslate(part.transform.translate);
            part.obj->SetRotate(part.transform.rotate);
        }
        part.obj->Update();
    }
}

void GameOverScene::UpdateUI() {
    if (!FadeManager::GetInstance()->IsFading() && FadeManager::GetInstance()->IsFadeEnd()) {
        switch (phase_) {
        case 0: // --- UI1移動中 ---
            if (ui1Timer_ < ui1Duration_) {
                ui1Timer_++;

                float t = ui1Timer_ / ui1Duration_;
                float easeT = EaseOutBack(t);

                Vector2 newPos1 = {
                    ui1StartPos.x + (ui1EndPos.x - ui1StartPos.x) * easeT,
                    ui1StartPos.y + (ui1EndPos.y - ui1StartPos.y) * easeT
                };
                ui1_->SetPosition(newPos1);
            }

            // --- 到着チェック ---
            if (fabsf(ui1_->GetPosition().y - ui1EndPos.y) < 0.5f) {
                ui1_->SetPosition(ui1EndPos);
                phase_ = 1;     // 次のUIへ
                ui2Timer_ = 0;  // 次のタイマーリセット
            }
            ui1_->Update();
            break;

        case 1: // --- UI2移動中 ---
            if (ui2Timer_ < ui2Duration_) {
                ui2Timer_++;

                float t = ui2Timer_ / ui2Duration_;
                float easeT = EaseOutBack(t);

                Vector2 newPos2 = {
                    ui2StartPos.x + (ui2EndPos.x - ui2StartPos.x) * easeT,
                    ui2StartPos.y + (ui2EndPos.y - ui2StartPos.y) * easeT
                };
                ui2_->SetPosition(newPos2);
            }

            if (fabsf(ui2_->GetPosition().y - ui2EndPos.y) < 0.5f) {
                ui2_->SetPosition(ui2EndPos);
                phase_ = 2; // 完了
            }
            ui2_->Update();
            break;

        case 2:
            // --- 全て完了 ---
            break;
        }
    }
}