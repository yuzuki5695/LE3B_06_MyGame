#include "GamePlayUI.h"
#include <TextureManager.h>
#include <Input.h>
#include <Easing.h>
#include <Player.h>
#include <Tools/AssetGenerator/engine/generated/ResourceID.h>
#include <EventManager.h>
#include <CameraManager.h>


using namespace ResourceID::texture;
using namespace Easing;

void GamePlayUI::Initialize() {

    TextureManager::GetInstance()->LoadTexture(Operationui::W);
    TextureManager::GetInstance()->LoadTexture(Operationui::A);
    TextureManager::GetInstance()->LoadTexture(Operationui::S);
    TextureManager::GetInstance()->LoadTexture(Operationui::D);
    TextureManager::GetInstance()->LoadTexture(Operationui::ArrowUp);
    TextureManager::GetInstance()->LoadTexture(Operationui::ArrowLeft);
    TextureManager::GetInstance()->LoadTexture(Operationui::ArrowDown);
    TextureManager::GetInstance()->LoadTexture(Operationui::ArrowRight);
    TextureManager::GetInstance()->LoadTexture(Operationui::SPACEKey);
    TextureManager::GetInstance()->LoadTexture(Operationui::SHIFT);

    TextureManager::GetInstance()->LoadTexture(Operationui::W_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::A_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::S_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::D_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::ArrowUp_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::ArrowLeft_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::ArrowDown_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::ArrowRight_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::SPACEKey_RED);
    TextureManager::GetInstance()->LoadTexture(Operationui::SHIFT_RED);

    
    TextureManager::GetInstance()->LoadTexture(Ui::Gage);
    TextureManager::GetInstance()->LoadTexture(Ui::Player_ui);

    gage_ = Sprite::Create(Ui::Gage, Vector2{ 380.0f, 10.0f }, 0.0f, Vector2{ 500.0f,30.0f });
    player_ui_ = Sprite::Create(Ui::Player_ui, Vector2{ 380.0f, 12.3f }, 0.0f, Vector2{ 25.0f,25.0f });

    // 2. UI生成
    Vector2 size = { 40.0f, 40.0f };
    Vector2 center = { 85.0f, 470.0f };
    CreateWASDUI(center, size, 4.0f, 25.0f);

    // 3. 初期状態設定（サイズ0）
    for (auto& ui : uis_) {
        uiOriginalSizes_.push_back(ui->GetSize());
        ui->SetAnchorPoint({ 0.5f, 0.5f });
        ui->SetSize({ 0.0f, 0.0f });
    }

    isAnimating_ = false;
    timer_ = 0.0f;
    duration_ = 0.5f;
}

void GamePlayUI::Update() { 
    // ===== UI進行更新 =====
    UpdateStageProgressUI();

    gage_->Update();
    player_ui_->Update();



    UpdateControlUIAnimation();
    UpdateControlUI(); 
    for (auto& ui : uis_) {
        ui->Update();
    }
}

void GamePlayUI::Draw() {
    
    gage_->Draw();
    player_ui_->Draw();

    for (auto& ui : uis_) {
        ui->Draw();
    }
}


void GamePlayUI::StartStageProgressUI() {
    if (uiProgressStarted_) return;
    uiStartRailLength_ = CameraManager::GetInstance()->GetGameplayCamera()->GetCurrentRailLength();

    uiProgressStarted_ = true;
}

void GamePlayUI::UpdateStageProgressUI() {
    if (!uiProgressStarted_ || uiProgressFinished_) return;

    auto* cam = CameraManager::GetInstance()->GetGameplayCamera();

    float current = cam->GetCurrentRailLength();
    float total   = cam->GetTotalRailLength();

    float uiTotalLength = total - uiStartRailLength_;
    if (uiTotalLength <= 0.0001f) return;

    float progress = (current - uiStartRailLength_) / uiTotalLength;

    if (progress >= 1.0f) {
        progress = 1.0f;
        uiProgressFinished_ = true;
    }

    progress = std::clamp(progress, 0.0f, 1.0f);

    constexpr float gageX = 380.0f;
    constexpr float gageWidth = 500.0f;
    constexpr float playerWidth = 25.0f;

    float movableWidth = gageWidth - playerWidth;
    float uiX = gageX + progress * movableWidth;

    player_ui_->SetPosition(Vector2{ uiX, 12.3f });
}

void GamePlayUI::StartControlUIAnimation() {
    if (!isAnimating_) {
        isAnimating_ = true;
    }
}

void GamePlayUI::CreateWASDUI(const Vector2& baseCenter, const Vector2& size, float keySpacing, float groupSpacing) {
    uis_.clear();

    float groupHeight = size.y * 2 + keySpacing;
    // W段 + ASD段の高さ

    // =====================
    // 1. WASD グループ
    // =====================
    Vector2 wasdCenter = baseCenter;

    Vector2 wPos = { wasdCenter.x, wasdCenter.y };
    Vector2 sRowY = { 0.0f,wasdCenter.y + size.y + keySpacing };

    Vector2 aPos = { wasdCenter.x - size.x - keySpacing, sRowY.y };
    Vector2 sPos = { wasdCenter.x, sRowY.y };
    Vector2 dPos = { wasdCenter.x + size.x + keySpacing, sRowY.y };

    uis_.push_back(Sprite::Create(Operationui::W, wPos, 0.0f, size)); //0
    uis_.push_back(Sprite::Create(Operationui::A, aPos, 0.0f, size)); //1
    uis_.push_back(Sprite::Create(Operationui::S, sPos, 0.0f, size)); //2
    uis_.push_back(Sprite::Create(Operationui::D, dPos, 0.0f, size)); //3


    // =====================
    // 2. Arrow グループ
    // =====================
    float arrowTopY = wasdCenter.y + groupHeight + groupSpacing;

    Vector2 arrowTop = { baseCenter.x, arrowTopY };
    float arrowRowY = arrowTopY + size.y + keySpacing;

    Vector2 upPos = { arrowTop.x, arrowTop.y };
    Vector2 leftPos = { arrowTop.x - size.x - keySpacing, arrowRowY };
    Vector2 downPos = { arrowTop.x, arrowRowY };
    Vector2 rightPos = { arrowTop.x + size.x + keySpacing, arrowRowY };

    uis_.push_back(Sprite::Create(Operationui::ArrowUp, upPos, 0.0f, size));     //4
    uis_.push_back(Sprite::Create(Operationui::ArrowRight, leftPos, 0.0f, size)); //5
    uis_.push_back(Sprite::Create(Operationui::ArrowDown, downPos, 0.0f, size)); //6
    uis_.push_back(Sprite::Create(Operationui::ArrowLeft, rightPos, 0.0f, size));//7


    // =====================
    // 3. SPACE
    // =====================
    Vector2 spaceSize = { 80.0f, 40.0f };
    float spaceY = arrowTopY + groupHeight + groupSpacing;
    // 1つ目（中央）
    Vector2 spacePos = { baseCenter.x - (spaceSize.x + keySpacing) * 0.5f, spaceY };

    // 2つ目（右側）
    Vector2 spacePos2 = { baseCenter.x + (spaceSize.x + keySpacing) * 0.5f, spaceY };

    uis_.push_back(Sprite::Create(Operationui::SPACEKey, spacePos, 0.0f, spaceSize));  //8
    uis_.push_back(Sprite::Create(Operationui::SPACEKey, spacePos2, 0.0f, spaceSize)); //9
}

void GamePlayUI::UpdateControlUI() {
    Input* input = Input::GetInstance();
    if (EventManager::GetInstance()->IsFinished()) {
        // === WASD ===
        uis_[0]->SetTexture(input->Pushkey(DIK_W) ? Operationui::W_RED : Operationui::W);
        uis_[1]->SetTexture(input->Pushkey(DIK_A) ? Operationui::A_RED : Operationui::A);
        uis_[2]->SetTexture(input->Pushkey(DIK_S) ? Operationui::S_RED : Operationui::S);
        uis_[3]->SetTexture(input->Pushkey(DIK_D) ? Operationui::D_RED : Operationui::D);

        // === Arrow ===
        uis_[4]->SetTexture(input->Pushkey(DIK_UP) ? Operationui::ArrowUp_RED : Operationui::ArrowUp);
        uis_[5]->SetTexture(input->Pushkey(DIK_RIGHT) ? Operationui::ArrowRight_RED : Operationui::ArrowRight);
        uis_[6]->SetTexture(input->Pushkey(DIK_DOWN) ? Operationui::ArrowDown_RED : Operationui::ArrowDown);
        uis_[7]->SetTexture(input->Pushkey(DIK_LEFT) ? Operationui::ArrowLeft_RED : Operationui::ArrowLeft);

        // === SPACE / SHIFT ===
        uis_[8]->SetTexture(input->Pushkey(DIK_SPACE) ? Operationui::SPACEKey_RED : Operationui::SPACEKey);
        uis_[9]->SetTexture(input->Pushkey(DIK_LSHIFT) ? Operationui::SHIFT_RED : Operationui::SHIFT);
    }
}

void GamePlayUI::UpdateControlUIAnimation() {
    if (!isAnimating_) return;

    timer_ += 1.0f / 60.0f;
    float t = std::min(timer_ / duration_, 1.0f);
    float ease = Easing::EaseOutBack(t);
    if (t >= 1.0f) {
        t = 1.0f;
        isAnimating_ = false;
    }

    for (size_t i = 0; i < uis_.size(); ++i) {
        float w = Easing::Lerp(0.0f, uiOriginalSizes_[i].x, ease);
        float h = Easing::Lerp(0.0f, uiOriginalSizes_[i].y, ease);
        uis_[i]->SetSize({ w, h });
    }

    if (t >= 1.0f) isAnimating_ = false;
}