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

// シングルトン用インスタンス
std::unique_ptr<FadeManager> FadeManager::instance = nullptr;

///====================================================
/// シングルトンインスタンスの取得
///====================================================
FadeManager* FadeManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<FadeManager>();
    }
    return instance.get();
}
///====================================================
/// 終了処理
///====================================================
void FadeManager::Finalize() {
    instance.reset();  // `delete` 不要
}
///====================================================
/// 初期化処理
///====================================================
void FadeManager::Initialize() {
    // テクスチャの読み込み
    TextureManager::GetInstance()->LoadTexture("fade/Black.png");
    TextureManager::GetInstance()->LoadTexture("fade/white.png");
    // フェード用スプライトの初期化
    sprite_ = Sprite::Create("fade/Black.png", Vector2{ 0.0f, 0.0f }, 0.0f, Vector2{ 1280.0f,720.0f });
    sprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f }); // 透明スタート         

    // 各種初期値
    timer_ = 0.0f;
    duration_ = 1.0f;
    alpha_ = 0.0f;
    fadeStart_ = false;
    fadeEnd_ = false;
    t_ = 0.0f;

    // 画面を格子状に黒丸で埋める
    const int cols = 10;
    const int rows = 6;
    const float spacingX = 1280.0f / cols;
    const float spacingY = 720.0f / rows; 
    const float maxDelay = 0.7f; // 最大遅延時間（秒）   
    Vector2 center = { 1280.0f / 2, 720.0f / 2 }; // 画面中心
    shapes_.clear(); 
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            FadeShape shape;
            shape.position = { (x + 0.5f) * spacingX, (y + 0.5f) * spacingY };
            shape.size = { spacingX, spacingY };
            shape.scale = 0.0f;

            shape.sprite = Sprite::Create("fade/white.png", shape.position, 0.0f, shape.size);
            shape.sprite->SetAnchorPoint({ 0.5f, 0.5f });
            shape.sprite->SetColor({ 0, 0, 0, 0 });

            // 中心からの距離に応じてdelay設定
            float dx = shape.position.x - center.x;
            float dy = shape.position.y - center.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            float maxDist = std::sqrt(center.x * center.x + center.y * center.y);
            shape.delay = (dist / maxDist) * maxDelay;
            shape.rotation = 0.0f;
            shape.rotationSpeed = (rand() % 2 == 0 ? 1 : -1) * (0.02f + (rand() % 100 / 5000.0f));
            shapes_.push_back(std::move(shape));
        }
    }
}
/// =========================================================
/// フェードイン開始
/// =========================================================
void FadeManager::StartFadeIn(float duration, FadeStyle style) {
    fadeType_ = FadeType::FadeIn;
    fadeStyle_ = style;
    duration_ = duration;
    duration_ = std::max(duration, 0.001f);
    t_ = 0.0f; 
    timer_ = 0.0f;
    alpha_ = 0.0f;
    fadeStart_ = true;
    fadeEnd_ = false;
    isFading_ = true;
}
/// =========================================================
/// フェードアウト開始
/// =========================================================
void FadeManager::StartFadeOut(float duration, FadeStyle style) {
    fadeType_ = FadeType::FadeOut;
    fadeStyle_ = style;
    duration_ = duration;
    duration_ = std::max(duration, 0.001f);
    t_ = 0.0f;
    timer_ = 0.0f;
    alpha_ = 0.0f;
    fadeStart_ = true;
    fadeEnd_ = false;
    isFading_ = true;
}
/// =========================================================
/// 更新処理
/// =========================================================
void FadeManager::Update() {
    if (!isFading_) return;

    // 経過時間を進める（60FPS前提）
    const float deltaTime = 1.0f / 60.0f;
    timer_ += deltaTime;

    // 経過割合を求める
    t_ = std::clamp(timer_ / duration_, 0.0f, 1.0f);
        
    // スタイル別処理
    switch (fadeStyle_) {
    case FadeStyle::Normal: UpdateNormalFade(); break;
    case FadeStyle::SilhouetteSlide: UpdateSilhouetteSlideFade(); break;
    case FadeStyle::SilhouetteExplode: UpdateSilhouetteExplodeFade(); break;   
    }

    // フェード完了判定
    if (t_ >= 1.0f) {
        // フラグリセット
        isFading_ = false;
        fadeEnd_ = true;
    }
}
/// =========================================================
///  通常フェードの更新処理
/// =========================================================
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
/// =========================================================
/// スライドフェードの更新処理
/// =========================================================
void FadeManager::UpdateSilhouetteSlideFade() {
    const float maxScale = 1.0f;
    auto easeOutQuad = [](float t) { return 1 - (1 - t) * (1 - t); };

    for (auto& s : shapes_) {
        float localT = std::clamp((t_ - s.delay) / (1.0f - s.delay), 0.0f, 1.0f);
        float easedT = easeOutQuad(localT);

        float scale, alpha;
        if (fadeType_ == FadeType::FadeOut) {
            scale = easedT * maxScale;
            alpha = easedT;
        } else {
            scale = maxScale - easedT * maxScale;
            alpha = 1.0f - easedT;
        }

        s.scale = scale;
        s.sprite->SetSize(s.size * s.scale);
        s.sprite->SetColor({ 0,0,0,alpha });
        s.sprite->Update();
    }
}
// =========================================================
// 爆発フェードの更新処理
// =========================================================
void FadeManager::UpdateSilhouetteExplodeFade() {
    const Vector2 center = { 1280.0f / 2, 720.0f / 2 };

    // 弾けるようなイージング
    auto easeOutElastic = [](float t) {
        const float c4 = (2 * 3.141592f) / 3;
        return (t == 0.0f) ? 0.0f :
            (t == 1.0f) ? 1.0f :
            powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
        };

    for (auto& s : shapes_) {
        float localT = std::clamp((t_ - s.delay) / (1.0f - s.delay), 0.0f, 1.0f);
        float easedT = easeOutElastic(localT);

        float scale = 1.0f;
        float alpha = 1.0f;
        Vector2 pos = s.position;
        Vector2 dir = { pos.x - center.x, pos.y - center.y };
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.0f) { dir.x /= len; dir.y /= len; }

        Vector4 color = { 0, 0, 0, 1 }; // デフォルト黒

        // ===============================================
        // フェードアウト処理（中心に戻る）
        // ===============================================
        if (fadeType_ == FadeType::FadeOut) {

            float spread = (1.0f - easedT) * 150.0f;
            Vector2 movedPos = { pos.x + dir.x * spread, pos.y + dir.y * spread };
            s.sprite->SetPosition(movedPos);

            scale = 1.0f - (1.0f - easedT) * 0.2f;
            alpha = easedT;
            color = { 0, 0, 0, alpha };
        }

        // ===============================================
        // フェードイン処理（黒→白→爆発移動）
        // ===============================================
        else {
            // 0〜0.3：黒→白へ（静止）
            if (localT < 0.3f) {
                float colorT = localT / 0.3f; // 0→1
                color = {
                    colorT, colorT, colorT, 1.0f // 黒→白
                };
                s.sprite->SetPosition(pos);  // 動かない
                scale = 1.0f;
                alpha = 1.0f;
            }
            // 0.3〜1.0：白→黒に戻りながら爆発的に移動
            else {
                float moveT = (localT - 0.3f) / 0.7f; // 0〜1へ正規化
                float easedMoveT = easeOutElastic(moveT);

                // 拡散距離（爆発）
                float spread = easedMoveT * 150.0f;
                Vector2 movedPos = { pos.x + dir.x * spread, pos.y + dir.y * spread };

                s.rotation += s.rotationSpeed * easedMoveT;
                s.sprite->SetRotation(s.rotation);
                s.sprite->SetPosition(movedPos);

                // スケール・アルファ変化
                scale = 1.0f + easedMoveT * 0.3f;

                // 完全に透明までフェードアウト
                alpha = std::clamp(1.0f - easedMoveT, 0.0f, 1.0f);


                // 白のまま固定
                color = { 1.0f, 1.0f, 1.0f, alpha };
            }
        }

        // ===============================================
        // 共通適用
        // ===============================================
        s.scale = scale;
        s.sprite->SetSize(s.size * s.scale);
        s.sprite->SetColor(color);
        s.sprite->Update();
    }
}
/// =========================================================
/// 描画処理
/// =========================================================
void FadeManager::Draw() {
    switch (fadeStyle_) {
    case FadeStyle::Normal:
        sprite_->Draw();
        break;
    case FadeStyle::SilhouetteSlide:
        for (auto& s : shapes_) {
            s.sprite->Draw();
        };
        break;
    case FadeStyle::SilhouetteExplode:
        for (auto& s : shapes_) {
            s.sprite->Draw();
        };
        break;
    }
}
/// =========================================================
/// ImGuiデバッグ描画（コメントアウト）
/// =========================================================
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