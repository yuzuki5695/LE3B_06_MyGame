#include "Pausemenu.h"
#include <TextureManager.h>
#include <Input.h>
#include <Easing.h>
#include <SceneManager.h>
#include <FadeManager.h>
#include <ImGuiManager.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <Tools/AssetGenerator/engine/generated/ResourceID.h>

using namespace ResourceID;
using namespace Easing;

void Pausemenu::Initialize() {
    TextureManager::GetInstance()->LoadTexture(texture::Pausemenu::PauseMenuPanel);
    TextureManager::GetInstance()->LoadTexture(texture::Pausemenu::Poseicon);
    TextureManager::GetInstance()->LoadTexture(texture::Pausemenu::Poseicon_red);   
    TextureManager::GetInstance()->LoadTexture(texture::Pausemenu::menu_01);
    TextureManager::GetInstance()->LoadTexture(texture::Pausemenu::menu_02);
    TextureManager::GetInstance()->LoadTexture(texture::Pausemenu::menu_03);
    TextureManager::GetInstance()->LoadTexture(texture::Pausemenu::menu_04);
    TextureManager::GetInstance()->LoadTexture(texture::Pausemenu::Arrow);

    // 初期状態は非アクティブ
    isActive_ = false;
	// ポーズアイコンの作成
    paused_ = Sprite::Create(texture::Pausemenu::Poseicon, Vector2{ 3.0f, 3.0f }, 0.0f, Vector2{ 100.0f,100.0f });
    
    isOpening_ = false; 
    isClosing_ = false;
    openTimer_ = 0.0f;
    // パネルUIの作成
    for (uint32_t i = 0; i < 2; i++) {
        UIElement element;
        element.size = { 700.0f, 400.0f };
        element.position = { 640.0f, 350.0f };
        element.position.x += (350.0f + 700.0f) * i; // 横にずらす
        // 初期位置（画面下）
        Vector2 startPos = element.position;
        startPos.y = 1000.0f;

        element.sprite = Sprite::Create(texture::Pausemenu::PauseMenuPanel, startPos, 0.0f, element.size);
        element.sprite->SetAnchorPoint({ 0.5f,0.5f });
        if (i == 1) {
            element.sprite->SetTextureLeftTop({ 700.0f,0.0f });
        }
        panels_.push_back(std::move(element));
    }
    
    // テクスチャ読み込み
    std::string textures[] = { 
        texture::Pausemenu::menu_01, texture::Pausemenu::menu_03,
        texture::Pausemenu::menu_02, texture::Pausemenu::menu_04    
    };
	// コマンドUIの作成
    for (uint32_t i = 0; i < kMaxCommand; ++i) {
        UIElement command;
        command.size = { 200.0f,70.0f };
        if (i == static_cast<uint32_t>(PauseCommand::Turnback)) {
            commandPanelIndex_[i] = 1;
        } else {
            commandPanelIndex_[i] = 0;
        }
        if (i == 3) {
            commandOffset_[i] = { 0.0f,((i - 1) * kMenuIntervalY) };
        } else {
            commandOffset_[i] = { 0.0f,(i * kMenuIntervalY) };
        }
        Vector2 panelPos = panels_[commandPanelIndex_[i]].sprite->GetPosition();
        Vector2 pos{ panelPos.x,panelPos.y + commandOffset_[i].y - 20.0f };
        command.sprite = Sprite::Create(textures[i], pos, 0.0f, command.size);
        command.sprite->SetAnchorPoint({ 0.5f,0.5f });
        commands_[i].push_back(std::move(command));
    }

    arrow_.size = { 50.0f,50.0f };
    arrow_.position = { commands_[selectedIndex_][0].sprite->GetPosition().x + kArrowOffsetX , commands_[selectedIndex_][0].sprite->GetPosition().y };
    arrow_.sprite = Sprite::Create(texture::Pausemenu::Arrow, arrow_.position, 0.0f, arrow_.size);
    arrow_.sprite->SetAnchorPoint(Vector2{ 0.5f,0.5f });

    selectedIndex_ = 0;
    fige = false;
}

void Pausemenu::Update() {
    if (isOpening_) {
        openTimer_++;
        float t = std::clamp(openTimer_ / kOpenMaxFrame, 0.0f, 1.0f);
        // 既存のイージング関数を使用
        float easeT = EaseOutBack(t);


        for (uint32_t i = 0; i < panels_.size(); i++) {

            Vector2 targetPos = panels_[i].position;
            Vector2 targetSize = panels_[i].size;

            float startY = 1000.0f;

            float currentY = startY + (targetPos.y - startY) * easeT;

            // サイズ補間
            Vector2 currentSize{
                targetSize.x * easeT,
                targetSize.y * easeT
            };

            panels_[i].sprite->SetPosition({ targetPos.x, currentY });
            panels_[i].sprite->SetSize(currentSize);
        }
        if (openTimer_ >= kOpenMaxFrame) {
            isOpening_ = false;
            fige = true;
        }
    }
    // 閉じるアニメーション
    if (isClosing_) {

        openTimer_++;

        float t = std::clamp(openTimer_ / kOpenMaxFrame, 0.0f, 1.0f);
        float easeT = EaseOutBack(t);
        float reverseT = 1.0f - easeT;

        for (UIElement& panel : panels_) {

            Vector2 targetSize = panel.size;

            float currentY = std::lerp(panel.position.y, 1000.0f, easeT);

            Vector2 currentSize{
                targetSize.x * reverseT,
                targetSize.y * reverseT
            };

            panel.sprite->SetPosition({ panel.position.x, currentY });
            panel.sprite->SetSize(currentSize);
        }

        if (openTimer_ >= kOpenMaxFrame) {

            isClosing_ = false;
            isActive_ = false; // ここでポーズ解除
        }
    }

    if (isActive_) {
        paused_->SetTexture(texture::Pausemenu::Poseicon_red);
        keyUpdate();
        ExecuteCommand(); // コマンド実行
    } else {
        paused_->SetTexture(texture::Pausemenu::Poseicon);
    }

    if (isOperationMove_) {
        UpdateOperationMove();
    }

    paused_->Update();
    arrow_.sprite->Update();

    for (UIElement& panel : panels_) {
        panel.sprite->Update();
    }

    for (std::vector<UIElement>& commandList : commands_) {
        for (UIElement& command : commandList) {
            command.sprite->Update();
        }
    }
}

void Pausemenu::Draw() {
    paused_->Draw();

    for (UIElement& panel : panels_) {
        panel.sprite->Draw();
    }

    for (std::vector<UIElement>& commandList : commands_) {
        for (UIElement& command : commandList) {
            command.sprite->Draw();
        }
    }
    
    arrow_.sprite->Draw();
}

void Pausemenu::keyUpdate() {
    // コマンド実行中は選択処理をしない
    if (!isCommandRunning_) {

        // 下
        if (Input::GetInstance()->Triggrkey(DIK_DOWN)) {
            selectedIndex_++;
            if (!isOperationMode_ && selectedIndex_ == static_cast<uint32_t>(PauseCommand::Turnback)) {
                selectedIndex_ = 0;
            }
            if (selectedIndex_ >= kMaxCommand) {
                selectedIndex_ = 0;
            }
            UpdateArrowPositions();
        }

        // 上
        if (Input::GetInstance()->Triggrkey(DIK_UP)) {
            if (selectedIndex_ == 0) {
                selectedIndex_ = kMaxCommand - 1;
            } else {
                selectedIndex_--;
            }

            if (!isOperationMode_ && selectedIndex_ == static_cast<uint32_t>(PauseCommand::Turnback)) {
                selectedIndex_--;
            }
            UpdateArrowPositions();
        }
    }
}

void Pausemenu::UpdateArrowPositions() {
    if (commands_[selectedIndex_].empty()) {
        return;
    }
    Vector2 targetPos = commands_[selectedIndex_][0].sprite->GetPosition();

    arrow_.sprite->SetPosition(Vector2{ targetPos.x + kArrowOffsetX,targetPos.y });
}

void Pausemenu::ExecuteCommand() { 

    if (!isOperationMove_ && Input::GetInstance()->Triggrkey(DIK_RETURN)) {
        command_ = static_cast<PauseCommand>(selectedIndex_);

        switch (command_) {
        case PauseCommand::Resume:
            if (fige) {
                isClosing_ = true;
                isOpening_ = false;
                openTimer_ = 0.0f;
                fige = false;
            }
            break;
        case PauseCommand::Operation:
            isOperationMode_ = true;
            isCommandRunning_ = true;
            isOperationMove_ = true;
            operationTimer_ = 0.0f;
            panel1Start_ = { kPanelCenterX, panels_[0].sprite->GetPosition().y };
            panel1End_ = { kPanelLeftX,   panels_[0].sprite->GetPosition().y };

            panel2Start_ = { kPanelRightX, panels_[1].sprite->GetPosition().y };
            panel2End_ = { kPanelCenterX, panels_[1].sprite->GetPosition().y };
        
            for (uint32_t i = 0; i < kMaxCommand; i++) {
                if (commands_[i].empty()) continue;

                Vector2 pos = commands_[i][0].sprite->GetPosition();
                commandStart_[i] = pos;

                // Turnbackだけパネル2へ
                if (i == static_cast<uint32_t>(PauseCommand::Turnback)) {
                    commandEnd_[i] = {
                        kPanelCenterX,
                        pos.y
                    };
                } else {
                    // panel1と一緒に左へ
                    commandEnd_[i] = {
                        pos.x + (kPanelLeftX - kPanelCenterX),
                        pos.y
                    };
                }
            }
            selectedIndex_ = static_cast<uint32_t>(PauseCommand::Turnback);
            UpdateArrowPositions();
            break;
        case PauseCommand::GoToTitle:
            break;
        case PauseCommand::Turnback:
            if (isOperationMode_) {

                isOperationMode_ = false;
                isCommandRunning_ = false;

                isOperationMove_ = true;
                operationTimer_ = 0.0f;

                panel1Start_ = { kPanelLeftX, panels_[0].sprite->GetPosition().y };
                panel1End_ = { kPanelCenterX, panels_[0].sprite->GetPosition().y };

                panel2Start_ = { kPanelCenterX, panels_[1].sprite->GetPosition().y };
                panel2End_ = { kPanelRightX, panels_[1].sprite->GetPosition().y };

                for (uint32_t i = 0; i < kMaxCommand; i++) {

                    if (commands_[i].empty()) continue;

                    Vector2 pos = commands_[i][0].sprite->GetPosition();
                    commandStart_[i] = pos;

                    commandEnd_[i] = {
                        640.0f,
                        kMenuTopY + (i * kMenuIntervalY)
                    };
                }

                selectedIndex_ = static_cast<uint32_t>(PauseCommand::Operation);
                UpdateArrowPositions();
            }

            break;
        default:
            break;
        }
    }

    for (uint32_t i = 0; i < kMaxCommand; i++) {

        if (commands_[i].empty()) {
            continue;
        }

        UIElement& command = commands_[i][0];

        // Turnback以外
        if (i == selectedIndex_ && i != static_cast<uint32_t>(PauseCommand::Turnback)) {
            command.sprite->SetTextureLeftTop({ 200.0f, 0.0f });
        } else {
            command.sprite->SetTextureLeftTop({ 0.0f, 0.0f });
        }
    }
    UpdateCommandPosition();
}

void Pausemenu::UpdateOperationMove() {
    operationTimer_ += 1.0f / 60.0f;

    float t = operationTimer_ / operationDuration_;

    if (t >= 1.0f) {
        t = 1.0f;
        isOperationMove_ = false;
    }

    float ease = EaseOutBack(t);

    // パネル
    Vector2 p1{
        Lerp(panel1Start_.x, panel1End_.x, ease),
        Lerp(panel1Start_.y, panel1End_.y, ease)
    };

    Vector2 p2{
        Lerp(panel2Start_.x, panel2End_.x, ease),
        Lerp(panel2Start_.y, panel2End_.y, ease)
    };

    panels_[0].sprite->SetPosition(p1);
    panels_[1].sprite->SetPosition(p2);
}

void Pausemenu::UpdateCommandPosition() {
    for (uint32_t i = 0; i < kMaxCommand; i++) {

        if (commands_[i].empty()) continue;

        int panelIndex = commandPanelIndex_[i];

        Vector2 panelPos = panels_[panelIndex].sprite->GetPosition();

        Vector2 pos{
            panelPos.x,
            panelPos.y + commandOffset_[i].y
        };

        commands_[i][0].sprite->SetPosition(pos);
    }

    UpdateArrowPositions();
}