#include "GameClearUI.h"
#include <TextureManager.h>
#include <Easing.h>

// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Textures;
using namespace Easing;

namespace MyGame {
    void GameClearUI::Initialize() {
        TextureManager::GetInstance()->LoadTexture(Ui::Mission);
        TextureManager::GetInstance()->LoadTexture(Ui::Complete);
        TextureManager::GetInstance()->LoadTexture(Ui::UI_01);

        ui2Timer_ = 0.0f;
        ui2Duration_ = 60.0f;
        ui2StartPos = { 1280.0f, 10.0f }; // 上にずらした初期位置
        ui2EndPos = { 600.0f, 10.0f }; // 最終位置（現在の値） 

        ui3Timer_ = 0.0f;
        ui3Duration_ = 60.0f;
        ui3StartPos = { 1280.0f, 130.0f };
        ui3EndPos = { 800.0f, 130.0f };
     
        
        ui01Timer_ = 0.0f;
        ui01Duration_ = 60.0f;

        ui01StartPos_ = { 1500.0f, 450.0f };
        ui01EndPos_ = { 950.0f, 450.0f };

        ui_01_ = Sprite::Create(Ui::UI_01, ui01StartPos_, 0.0f, Vector2{ 360.0f,100.0f });
        ui_01_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準

        sprite_ = Sprite::Create(Ui::Mission, ui2StartPos, 0.0f, Vector2{ 400.0f,150.0f });
        complete_ = Sprite::Create(Ui::Complete, ui3StartPos, 0.0f, Vector2{ 400.0f,150.0f });
    }

    void GameClearUI::Update() {
        if (movestarted_) {
            if (ui2Timer_ < ui2Duration_ && sprite_->GetPosition().x != ui2EndPos.x) {
                ui2Timer_++;

                float t = ui2Timer_ / ui2Duration_;
                float easeT = EaseOutBack(t);
                Vector2 newPos1 = {
                    ui2StartPos.x + (ui2EndPos.x - ui2StartPos.x) * easeT,
                    ui2StartPos.y + (ui2EndPos.y - ui2StartPos.y) * easeT
                };
                sprite_->SetPosition(newPos1);
            }

            if (ui3Timer_ < ui3Duration_ && sprite_->GetPosition().x != ui3EndPos.x && sprite_->GetPosition().x < 900) {
                ui3Timer_++;

                float t = ui3Timer_ / ui3Duration_;
                float easeT = EaseOutBack(t);
                Vector2 newPos1 = {
                    ui3StartPos.x + (ui3EndPos.x - ui3StartPos.x) * easeT,
                    ui3StartPos.y + (ui3EndPos.y - ui3StartPos.y) * easeT
                };
                complete_->SetPosition(newPos1);
            } else if (ui3Timer_ >= ui3Duration_) {
                ui01Started_ = true;
            }

            if (ui01Started_) {
                if (ui01Timer_ < ui01Duration_) {
                    ui01Timer_++;
                    float t = ui01Timer_ / ui01Duration_;

                    float easeT = EaseOutBack(t);

                    Vector2 newPos = {
                        ui01StartPos_.x + (ui01EndPos_.x - ui01StartPos_.x) * easeT,
                        ui01StartPos_.y + (ui01EndPos_.y - ui01StartPos_.y) * easeT
                    };

                    ui_01_->SetPosition(newPos);
                    
                    if (t >= 1.0f) {
                        ui01Finished_ = true;
                    }
                }
            }
        }
        if (backstarted_) {

            if (backTimer_ < backDuration_) {

                backTimer_++;

                float t = backTimer_ / backDuration_;

                // 逆方向なので EaseInBack が合う
                float easeT = EaseInBack(t);

                // Mission
                Vector2 missionPos = {
                    ui2EndPos.x + (1500.0f - ui2EndPos.x) * easeT,
                    ui2EndPos.y
                };

                sprite_->SetPosition(missionPos);

                // Complete
                Vector2 completePos = {
                    ui3EndPos.x + (1500.0f - ui3EndPos.x) * easeT,
                    ui3EndPos.y
                };

                complete_->SetPosition(completePos);

                // UI_01
                Vector2 ui01Pos = {
                    ui01EndPos_.x + (ui01StartPos_.x - ui01EndPos_.x) * easeT,
                    ui01EndPos_.y
                };

                ui_01_->SetPosition(ui01Pos);
            }
        }


        sprite_->Update();
        complete_->Update();
        ui_01_->Update();
    }

    void GameClearUI::Draw() {
        sprite_->Draw();
        complete_->Draw();
        ui_01_->Draw();
    }
}