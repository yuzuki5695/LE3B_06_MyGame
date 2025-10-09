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

std::unique_ptr<FadeManager> FadeManager::instance = nullptr;

FadeManager* FadeManager::GetInstance() {
    if (!instance) instance = std::make_unique<FadeManager>();
    return instance.get();
}

void FadeManager::Initialize() {   
	// フェード用テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture("fade/Black.png");
    // フェード用スプライトの初期化
    sprite_ = Sprite::Create("fade/Black.png", Vector2{ 0.0f, 0.0f }, 0.0f, Vector2{ 1280.0f,720.0f });
    sprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f}); // 透明スタート
}

void FadeManager::StartFadeIn(float duration) {
    duration_ = std::max(duration, 0.001f); // ゼロ除算防止
    timer_ = 0.0f;
    alpha_ = 0.0f; // 透明から
    isFading_ = true;
}

void FadeManager::StartFadeOut(float duration) {
    fadeType_ = FadeType::FadeOut;
    isFading_ = true;
    duration_ = duration;
    timer_ = 0.0f;
    alpha_ = 0.0f; // 透明から始まる
}

void FadeManager::Update() {
    if (!isFading_) return;

    // 経過時間を進める（60FPS前提）
    const float deltaTime = 1.0f / 60.0f;
    timer_ += deltaTime;

    // 経過割合を求める
    float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);

    // フェードアウト（透明→黒）
    alpha_ = t;

    // スプライトに反映
    sprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha_ });

    // 完了判定
    if (t >= 1.0f) {
        isFading_ = false;
    }
    sprite_->Update();
}

void FadeManager::Draw() {
    if (!sprite_) return;
    sprite_->Draw();
}