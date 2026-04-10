#pragma once
#include <IFadeEffect.h>
#include <TextureManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>
#include <algorithm>

namespace MyGame {
    class ExplodeFade : public IFadeEffect {
	private:  
        std::vector<FadeShape> shapes_;
    public:
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

                    shape.sprite = MyEngine::Sprite::Create(AssetGen::LoadResourceID::Textures::fade::white, shape.position, 0.0f, shape.size);
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

        void Update(float t, FadeType type) override {
            const float maxScale = 1.0f;
            auto easeOutQuad = [](float t) { return 1 - (1 - t) * (1 - t); };

            for (auto& s : shapes_) {
                float localT = std::clamp((t - s.delay) / (1.0f - s.delay), 0.0f, 1.0f);
                float easedT = easeOutQuad(localT);

                float scale, alpha;
                if (type == FadeType::FadeOut) {
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

        void Draw() override {
            for (auto& s : shapes_) {
                s.sprite->Draw();
            };
        }
    };
}