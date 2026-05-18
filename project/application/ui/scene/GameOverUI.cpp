#include "GameOverUI.h"
#include <TextureManager.h>
#include <Easing.h>
#include <FadeManager.h>
#include <Input.h>
#include <SceneName.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Textures;
using namespace Easing;

namespace MyGame {

    void GameOverUI::Initialize() {
        // テクスチャの読み込み
        TextureManager::GetInstance()->LoadTexture(Ui::Gameover);
        TextureManager::GetInstance()->LoadTexture(Ui::UI_01);

        ui1Timer_ = 0.0f;
        ui1Duration_ = 60.0f;
        ui2Timer_ = 0.0f;
        ui2Duration_ = 60.0f;

        ui1StartPos = { 1000.0f, -100.0f }; // 初期位置
        ui1EndPos = { 1000.0f, 200.0f }; // 最終位置（現在の値）
        ui2StartPos = { 1000.0f, -250.0f };
        ui2EndPos = { 1000.0f, 350.0f };
            
        phase_ = 0;

        // ゲームオーバーUIのスプライトを生成,初期化
        gameover_ = Sprite::Create(Ui::Gameover, ui1StartPos, 0.0f, Vector2{ 300.0f,120.0f });
        gameover_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準
        // エンターUIのスプライトを生成,初期化
        enter_ = Sprite::Create(Ui::UI_01, ui2StartPos, 0.0f, Vector2{ 360.0f,100.0f });
        enter_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準
        gameover_->SetTextureSize(Vector2{ 300.0f,120.0f });
        enter_->SetTextureSize(Vector2{ 360.0f,100.0f });
    }

    void GameOverUI::Update() {
		// フェードインが完了していない場合はUIの移動を開始しない
        if (!FadeManager::GetInstance()->IsFadeInFinished()) { return; }
        
        // タイトルシーンへの入力処理
        if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
            // UI演出完了後のみキー入力を許可
            if (phase_ == 2) {
                // タイトルシーンへ遷移
                FadeManager::GetInstance()->SceneChangeFade(SceneName::TITLE, FadeStyle::SilhouetteSlide, 1.0f);
            }
        }

        // UIの移動を段階的に行うための状態管理
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
                gameover_->SetPosition(newPos1);
            }

            // --- 到着チェック ---
            if (fabsf(gameover_->GetPosition().y - ui1EndPos.y) < 0.5f) {
                gameover_->SetPosition(ui1EndPos);
                phase_ = 1;     // 次のUIへ
                ui2Timer_ = 0;  // 次のタイマーリセット
            }
            gameover_->Update();
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
                enter_->SetPosition(newPos2);
            }

            if (fabsf(enter_->GetPosition().y - ui2EndPos.y) < 0.5f) {
                enter_->SetPosition(ui2EndPos);
                phase_ = 2; // 完了
            }
            enter_->Update();
            break;

        case 2:
            // --- 全て完了 ---
            break;
        }
    }

    void GameOverUI::Draw() {	
		// ゲームオーバーUIの描画処理
        gameover_->Draw();
		// エンターUIの描画処理
        enter_->Draw();
    }
}