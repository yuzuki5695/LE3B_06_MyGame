#pragma once
#include <IFadeEffect.h>
#include <TextureManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>
#include <algorithm>

namespace MyGame {

    /// <summary>
    /// 中心から外に飛び散るようなフェードエフェクト
    /// </summary>
    class SlideFade : public IFadeEffect {
    public: // メンバ関数
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize() override {
            MyEngine::TextureManager::GetInstance()->LoadTexture(AssetGen::LoadResourceID::Textures::fade::white);
            // 画面を格子状に黒丸で埋める
            const int cols = 10;
            const int rows = 6;
            const float spacingX = 1280.0f / cols;
            const float spacingY = 720.0f / rows;
            const float maxDelay = 0.7f; // 最大遅延時間（秒）   
            MyEngine::Vector2 center = { 1280.0f / 2, 720.0f / 2 }; // 画面中心
            shapes_.clear();
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < cols; ++x) {
                    FadeShape shape;
                    shape.position = { (x + 0.5f) * spacingX, (y + 0.5f) * spacingY };
                    shape.size = { spacingX, spacingY };
                    shape.scale = 0.0f;

                    shape.mission_ = MyEngine::Sprite::Create(AssetGen::LoadResourceID::Textures::fade::white, shape.position, 0.0f, shape.size);
                    shape.mission_->SetAnchorPoint({ 0.5f, 0.5f });
                    shape.mission_->SetColor({ 0, 0, 0, 0 });

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
        /// <summary>
        /// 更新処理
        /// </summary>
        /// <param name="t"></param>
        /// <param name="type"></param>    
        void Update(float t, FadeType type) override {
            const MyEngine::Vector2 center = { 1280.0f / 2, 720.0f / 2 };

            // 弾けるようなイージング
            auto easeOutElastic = [](float t) {
                const float c4 = (2 * 3.141592f) / 3;
                return (t == 0.0f) ? 0.0f :
                    (t == 1.0f) ? 1.0f :
                    powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
                };

            for (auto& s : shapes_) {
                float localT = std::clamp((t - s.delay) / (1.0f - s.delay), 0.0f, 1.0f);
                float easedT = easeOutElastic(localT);

                float scale = 1.0f;
                float alpha = 1.0f;
                MyEngine::Vector2 pos = s.position;
                MyEngine::Vector2 dir = { pos.x - center.x, pos.y - center.y };
                float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                if (len > 0.0f) { dir.x /= len; dir.y /= len; }

                MyEngine::Vector4 color = { 0, 0, 0, 1 }; // デフォルト黒

                // ===============================================
                // フェードアウト処理（中心に戻る）
                // ===============================================
                if (type == FadeType::FadeOut) {

                    float spread = (1.0f - easedT) * 150.0f;
                    MyEngine::Vector2 movedPos = { pos.x + dir.x * spread, pos.y + dir.y * spread };
                    s.mission_->SetPosition(movedPos);

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
                        s.mission_->SetPosition(pos);  // 動かない
                        scale = 1.0f;
                        alpha = 1.0f;
                    }
                    // 0.3〜1.0：白→黒に戻りながら爆発的に移動
                    else {
                        float moveT = (localT - 0.3f) / 0.7f; // 0〜1へ正規化
                        float easedMoveT = easeOutElastic(moveT);

                        // 拡散距離（爆発）
                        float spread = easedMoveT * 150.0f;
                        MyEngine::Vector2 movedPos = { pos.x + dir.x * spread, pos.y + dir.y * spread };

                        s.rotation += s.rotationSpeed * easedMoveT;
                        s.mission_->SetRotation(s.rotation);
                        s.mission_->SetPosition(movedPos);

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
                s.mission_->SetSize(s.size * s.scale);
                s.mission_->SetColor(color);
                s.mission_->Update();
            }
        }
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw() override {
            for (auto& s : shapes_) {
                s.mission_->Draw();
            };
        }
    private:  // メンバ変数
        // フェード用スプライト群
        std::vector<FadeShape> shapes_;
    };
}