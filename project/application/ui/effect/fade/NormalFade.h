#pragma once
#include <IFadeEffect.h>
#include <TextureManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

namespace MyGame {
    class NormalFade : public IFadeEffect {
	private:
        std::unique_ptr<MyEngine::Sprite> sprite_;

    public:
        void Initialize() override {
            MyEngine::TextureManager::GetInstance()->LoadTexture(AssetGen::LoadResourceID::Textures::fade::Black);
            sprite_ = MyEngine::Sprite::Create(AssetGen::LoadResourceID::Textures::fade::Black, { 0,0 }, 0, { 1280,720 });
        }

        void Update(float t, FadeType type) override {
            float alpha = (type == FadeType::FadeOut) ? t : (1.0f - t);
            sprite_->SetColor({ 0,0,0,alpha });
            sprite_->Update();
        }

        void Draw() override {
            sprite_->Draw();
        }
    };
}