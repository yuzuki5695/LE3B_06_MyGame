#include "Pausemenu.h"
#include <TextureManager.h>
#include <Input.h>

void Pausemenu::Initialize() {
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/PauseMenuPanel.png");
    // タイトルロゴの生成
    ui_ = Sprite::Create("Textures/Ui/PauseMenuPanel.png", Vector2{ 300.0f, 100.0f }, 0.0f, Vector2{ 500.0f,200.0f });
    ui_->SetTextureSize(Vector2{ 500.0f,200.0f });
    command_ = PauseCommand::None;
    selectedIndex_ = 0;
}

void Pausemenu::Update() {
    //// 上下キーなどで選択項目を切り替える（例）
    //if (Input::GetInstance()->Triggrkey(DIK_UP) || Input::GetInstance()->Triggrkey(DIK_W)) {
    //    selectedIndex_--;
    //}
    //if (Input::GetInstance()->Triggrkey(DIK_DOWN) || Input::GetInstance()->Triggrkey(DIK_S)) {
    //    selectedIndex_++;
    //}

    //// 項目数の範囲に収める (0: 再開, 1: タイトル)
    //selectedIndex_ = (selectedIndex_ + 2) % 2;

    //// Enterで決定
    //if (Input::GetInstance()->Triggrkey(DIK_RETURN)) {
    //    if (selectedIndex_ == 0) {
    //        command_ = PauseCommand::Resume;
    //    } else {
    //        command_ = PauseCommand::GoToTitle;
    //    }
    //}


    ui_->Update();
}

void Pausemenu::Draw() {
    
    ui_->Draw();

}