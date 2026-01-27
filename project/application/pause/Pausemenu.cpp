#include "Pausemenu.h"
#include <TextureManager.h>
#include <Input.h>
#include<Easing.h>
#include <SceneManager.h>
#include <FadeManager.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace Easing;

void Pausemenu::Initialize() {
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/PauseMenuPanel.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/Arrow.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/command_01.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/command_02.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/Poseicon.png");
    command_ = PauseCommand::None;
    isActive_ = false;
    selectedIndex_ = 0;
    MAXselectedIndex_ = 2;
    frame_ = 0.0f;
    baseSize_ = { 700.0f, 400.0f }; // 最終的なサイズ 
    menupanel_ = Sprite::Create("Textures/Ui/PauseMenuPanel.png", Vector2{ 650.0f, 350.0f }, 0.0f, baseSize_);
    menupanel_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    arrow_.size = { 50.0f,50.0f };
    arrow_.sprite = Sprite::Create("Textures/Ui/Arrow.png", Vector2{ 650.0f, 350.0f }, 0.0f, arrow_.size);
    arrow_.sprite->SetAnchorPoint(Vector2{ 0.5f,0.5f });

    // テクスチャ読み込み
    std::string textures[] = { "Textures/Ui/command_01.png", "Textures/Ui/command_02.png" };
    for (uint32_t i = 0; i < kMaxCommand; ++i) {
        commands_[i].size = { 150.0f, 50.0f };
        // Y座標を間隔分ずらして配置
        Vector2 pos = { 640.0f, kMenuTopY + (i * kMenuIntervalY) };
        commands_[i].sprite = Sprite::Create(textures[i], pos, 0.0f, commands_[i].size);
        commands_[i].sprite->SetAnchorPoint({ 0.5f, 0.5f });
    }
    icon_ = Sprite::Create("Textures/Ui/Poseicon.png", Vector2{ 3.0f, 3.0f }, 0.0f, Vector2{ 100.0f,100.0f });
}

void Pausemenu::Update() {
    // --- 1. アニメーション（演出）の更新 ---
    if (isActive_) {
        if (frame_ < kMaxFrame + 20.0f) { // 全体の演出時間をディレイ分少し伸ばす
            frame_ += 1.0f;
        }
    } else {
        if (frame_ > 0.0f) {
            frame_ -= 1.0f;
        } else {
            isFinished_ = true;
        }
    }

    // --- 2. 各パーツのサイズ更新（時間差をつける） ---
        // 背景パネル：即座に開始
    CalculateEaseSize(menupanel_.get(), baseSize_, frame_, kMaxFrame);

    // コマンド：背景パネルより 10フレーム遅れて開始
    float buttonFrame = std::max(0.0f, frame_ - 10.0f);
    for (uint32_t i = 0; i < kMaxCommand; ++i) {
        CalculateEaseSize(commands_[i].sprite.get(), commands_[i].size, buttonFrame, kMaxFrame);

        // --- 選択中の色変更 ---
        if (isActive_ && i == selectedIndex_) {
            commands_[i].sprite->SetColor({ 1.0f, 1.0f, 0.5f, 1.0f }); // 選択中：黄色っぽく
        } else {
            commands_[i].sprite->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 非選択：白
        }
    }

    // 矢印：背景パネルより 15フレーム遅れて開始
    float arrowFrame = std::max(0.0f, frame_ - 10.0f);
    CalculateEaseSize(arrow_.sprite.get(), arrow_.size, arrowFrame, kMaxFrame);

    // 入力と位置の更新（完全に開ききっている時のみ操作可能）
    if (isActive_ && frame_ >= kMaxFrame) {
        UpdateSelection(); // コマンド選択
        UpdateArrowPositions(); // 矢印の処理
    }
    menupanel_->Update();
    arrow_.sprite->Update();
    for (UIElement& cmd : commands_) {
        cmd.sprite->Update(); // 全ての選択肢
    }
}

void Pausemenu::Draw() {

    menupanel_->Draw();
    for (UIElement& cmd : commands_) {
        cmd.sprite->Draw(); // 全ての選択肢
    }
    arrow_.sprite->Draw();
}

void Pausemenu::CalculateEaseSize(Sprite* sprite, const Vector2& baseSize, float frame, float maxFrame) {
    if (!sprite) return;

    float t = std::clamp(frame / maxFrame, 0.0f, 1.0f);
    float easeT = 0.0f;

    if (t == 0.0f) {
        easeT = 0.0f;
    } else if (t == 1.0f) {
        easeT = 1.0f;
    } else {
        // easeOutBack の計算式
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;
        easeT = 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
    }

    Vector2 currentSize = { baseSize.x * easeT, baseSize.y * easeT };
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
            // Resumeを選択: isActiveをfalseにして逆再生演出を開始させる
            isActive_ = false;
            command_ = PauseCommand::None; // まだゲーム側には通知しない（演出を待つため）
        } else if (selectedIndex_ == 1) {
            command_ = PauseCommand::GoToTitle;
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

void Pausemenu::IconUpdate() {
    icon_->Update();
}
 
void Pausemenu::IconDraw() {
    icon_->Draw();
}