#include "Pausemenu.h"
#include <TextureManager.h>
#include <Input.h>
#include<Easing.h>
#include <SceneManager.h>
#include <FadeManager.h>

using namespace Easing;

void Pausemenu::Initialize() {
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/PauseMenuPanel.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/Arrow.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/command_01.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/command_02.png");
    command_ = PauseCommand::None;
    isActive_ = false;
    selectedIndex_ = 0;
    MAXselectedIndex_ = 2;
    frame_ = 0.0f;
    baseSize_ = { 700.0f, 400.0f }; // 最終的なサイズ 
    ui_ = Sprite::Create("Textures/Ui/PauseMenuPanel.png", Vector2{ 650.0f, 350.0f }, 0.0f, baseSize_);
    ui_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    arrow_.size = { 50.0f,50.0f };
    arrow_.sprite = Sprite::Create("Textures/Ui/Arrow.png", Vector2{ 650.0f, 350.0f }, 0.0f, arrow_.size);
    arrow_.sprite->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    
    // テクスチャ読み込み
    std::string textures[] = { "Textures/Ui/command_01.png", "Textures/Ui/command_02.png" };
    for (uint32_t i = 0; i < kMaxCommand; ++i) {
        commands_[i].size = { 150.0f, 50.0f };
        // Y座標を間隔分ずらして配置
        Vector2 pos = { 650.0f, kMenuTopY + (i * kMenuIntervalY) };
        commands_[i].sprite = Sprite::Create(textures[i], pos, 0.0f, commands_[i].size);
        commands_[i].sprite->SetAnchorPoint({ 0.5f, 0.5f });
    }
}

void Pausemenu::Update() {
    // --- 1. フレームを進める
    if (frame_ < kMaxFrame) {
        frame_ += 1.0f;
    }
    CalculateEaseSize(ui_.get(), baseSize_, frame_, kMaxFrame);
     
    UpdateSelection(); // コマンド選択
    UpdateArrowPositions(); // 矢印の処理

    ui_->Update();
    arrow_.sprite->Update();
    for (UIElement& cmd : commands_) {
        cmd.sprite->Update(); // 全ての選択肢
    }
}

void Pausemenu::Draw() {

    ui_->Draw();
    for (UIElement& cmd : commands_) {
        cmd.sprite->Draw(); // 全ての選択肢
    }
    arrow_.sprite->Draw();
}

void Pausemenu::CalculateEaseSize(Sprite* sprite, const Vector2& baseSize, float frame, float maxFrame) {
    if (!sprite) return; // 安全策

    // 進捗率 (0.0 ～ 1.0)
    float t = std::clamp(frame / maxFrame, 0.0f, 1.0f);

    // イージング適用 (EaseOutQuart)
    float easeT = 1.0f - std::pow(1.0f - t, 4.0f);

    // サイズ計算と適用
    Vector2 currentSize = {
        baseSize.x * easeT,
        baseSize.y * easeT
    };
    sprite->SetSize(currentSize);
}

void Pausemenu::UpdateSelection() {
    // 入力による選択項目の切り替え
    if (Input::GetInstance()->Triggrkey(DIK_UP) || Input::GetInstance()->Triggrkey(DIK_W)) {
        selectedIndex_ = (selectedIndex_ - 1 + MAXselectedIndex_) % MAXselectedIndex_;
    }
    if (Input::GetInstance()->Triggrkey(DIK_DOWN) || Input::GetInstance()->Triggrkey(DIK_S)) {
        selectedIndex_ = (selectedIndex_ + 1) % MAXselectedIndex_;
    }
    // 決定
    if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
        if (selectedIndex_ == 0) {
            command_ = PauseCommand::Resume;
            isActive_ = false; // 閉じる演出へ
        } else if (selectedIndex_ == 1) {
            command_ = PauseCommand::GoToTitle;
            //FadeManager::GetInstance()->StartFadeOut(1.0f, FadeStyle::Normal);
            SceneManager::GetInstance()->ChangeScene("TITLE");
        }
    }
}
void Pausemenu::UpdateArrowPositions() {
    // 現在選ばれている項目の位置を取得
    Vector2 targetPos = commands_[selectedIndex_].sprite->GetPosition();

    // 矢印を項目の左側（または右側）に配置するためのオフセット
    // 例: 項目の中心から左に 120ピクセルずらす
    float offset = 120.0f;
    arrow_.sprite->SetPosition(Vector2{ targetPos.x + offset, targetPos.y });
}