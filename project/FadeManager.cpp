#include "FadeManager.h"
#include <TextureManager.h>
#define NOMINMAX
#include <Windows.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <algorithm>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

// 静的メンバ変数の定義
std::unique_ptr<FadeManager> FadeManager::instance = nullptr;

// シングルトンインスタンスの取得
FadeManager* FadeManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<FadeManager>();
    }
    return instance.get();
}

// 終了
void FadeManager::Finalize() {
    instance.reset();  // `delete` 不要
}

void FadeManager::Initialize() {
    // フェード用テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture("fade/Black.png");
    // フェード用スプライトの初期化
    sprite_ = Sprite::Create("fade/Black.png", Vector2{ 0.0f, 0.0f }, 0.0f, Vector2{ 1280.0f,720.0f });
    sprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f }); // 透明スタート         


    timer_ = 0.0f;
    duration_ = 1.0f;
    alpha_ = 0.0f;
    fadeStart_ = false;
    fadeEnd_ = false;
    t_ = 0.0f;

    TextureManager::GetInstance()->LoadTexture("uvChecker.png");
    // 画面を格子状に黒丸で埋める
    const int cols = 11;
    const int rows = 7;
    const float spacingX = 1280.0f / cols;
    const float spacingY = 720.0f / rows;
   
    const float maxDelay = 0.5f; // 最大遅延時間（秒）
    shapes_.clear();
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            FadeShape shape;

            // 中央アンカーなので、セルの中央に配置
            shape.position = { (x + 0.5f) * spacingX, (y + 0.5f) * spacingY };
            shape.size = { spacingX, spacingY };
            shape.scale = 0.0f;

            shape.sprite = Sprite::Create("uvChecker.png", shape.position, 0.0f, shape.size);
            shape.sprite->SetAnchorPoint({ 0.5f, 0.5f });
            shape.sprite->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });

            // 左から順に delay を設定
            shape.delay = (float)x / cols * maxDelay;

            shapes_.push_back(std::move(shape));
        }
    }

}

void FadeManager::StartFadeIn(float duration, FadeStyle style) {
    fadeType_ = FadeType::FadeIn;
    fadeStyle_ = style;
    duration_ = duration;
    duration_ = std::max(duration, 0.001f);
    t_ = 0.0f; 
    timer_ = 0.0f;
    alpha_ = 0.0f;
    fadeStart_ = true;
    isFading_ = true;
}

void FadeManager::StartFadeOut(float duration, FadeStyle style) {
    fadeType_ = FadeType::FadeOut;
    fadeStyle_ = style;
    duration_ = duration;
    duration_ = std::max(duration, 0.001f);
    t_ = 0.0f;
    timer_ = 0.0f;
    alpha_ = 0.0f;
    fadeStart_ = true;
    isFading_ = true;
}

void FadeManager::Update() {
    if (!isFading_) return;

    // 経過時間を進める（60FPS前提）
    const float deltaTime = 1.0f / 60.0f;
    timer_ += deltaTime;

    // 経過割合を求める
    t_ = std::clamp(timer_ / duration_, 0.0f, 1.0f);
        

    switch (fadeStyle_) {
    case FadeStyle::Normal: UpdateNormalFade(); break;
    case FadeStyle::Circle: UpdateSilhouetteFade(); break;
    }

    // フェード完了判定
    if (t_ >= 1.0f) {
        isFading_ = false;
        fadeEnd_ = true;
    }
}

void FadeManager::UpdateNormalFade() {
    if (fadeType_ == FadeType::FadeOut) {
        // 透明 → 黒
        alpha_ = t_;
    } else if (fadeType_ == FadeType::FadeIn) {
        // 黒 → 透明
        alpha_ = 1.0f - t_;
    }
    // スプライトに反映
    sprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha_ });
    sprite_->Update();
}

void FadeManager::UpdateSilhouetteFade() {
    const float maxScale = 1.0f;

    for (auto& s : shapes_) {
        float scale;
        float alpha;

        if (fadeType_ == FadeType::FadeOut) {
            // フェードアウト（左から右）
            float localT = std::clamp((t_ - s.delay) / (1.0f - s.delay), 0.0f, 1.0f);
            scale = localT * maxScale;
            alpha = localT;
        } else { // FadeIn（右から左）
            // delay を反転して右側から開始
            float reversedDelay = 1.0f - s.delay;
            float localT = std::clamp((t_ - reversedDelay) / (1.0f - reversedDelay), 0.0f, 1.0f);

            scale = maxScale - localT * maxScale;
            alpha = 1.0f - localT;
        }

        s.scale = scale;
        s.sprite->SetSize(s.size * s.scale);
        s.sprite->SetColor({ 0.0f, 0.0f, 0.0f, alpha });
        s.sprite->Update();
    }
}

void FadeManager::Draw() {
    switch (fadeStyle_) {
    case FadeStyle::Normal:
        sprite_->Draw();
        break;
    case FadeStyle::Circle:
        for (auto& s : shapes_) {
            s.sprite->Draw();
        };
        break;
    }
}

// 🔸 ImGui制御関数
void FadeManager::DrawImGui() {
#ifdef _DEBUG
    //ImGui::Begin("Fade Manager");

    //ImGui::Text("Fade Type: %s",
    //    fadeType_ == FadeType::FadeIn ? "FadeIn" :
    //    fadeType_ == FadeType::FadeOut ? "FadeOut" : "None");

    //ImGui::SliderFloat("Alpha", &alpha_, 0.0f, 1.0f);
    //ImGui::SliderFloat("Duration", &duration_, 0.1f, 5.0f);

    //if (ImGui::Button("Start Fade Out")) {
    //    StartFadeOut(duration_);
    //}
    //if (ImGui::Button("Start Fade In")) {
    //    StartFadeIn(duration_);
    //}

    //ImGui::Separator();
    //if (sprite_) {
    //    sprite_->DrawImGui();  // 👈 Sprite の ImGui 制御も呼ぶ
    //}

    //ImGui::End();
#endif
}