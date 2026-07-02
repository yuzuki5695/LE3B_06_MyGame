#pragma once
#include <IFadeEffect.h>
#include <TextureManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

namespace MyGame {
    /// <summary>
	/// 通常の全画面フェードエフェクト
    /// </summary>
    class NormalFade : public IFadeEffect {
	public: // メンバ関数
        /// <summary>
		/// 初期化処理
        /// </summary>
        void Initialize() override {
			// 黒テクスチャをロード
            MyEngine::TextureManager::GetInstance()->LoadTexture(AssetGen::LoadResourceID::Textures::fade::Black);
			// フェード用スプライトを生成
            sprite_ = MyEngine::Sprite::Create(AssetGen::LoadResourceID::Textures::fade::Black, { 0,0 }, 0, { 1280,720 });
        }
        /// <summary>
		/// 更新処理
        /// </summary>
        /// <param name="t"></param>
        /// <param name="type"></param>
        void Update(float t, FadeType type) override {
			// フェードの進行度に応じてスプライトのアルファ値を変更
            float alpha = (type == FadeType::FadeOut) ? t : (1.0f - t);
			// スプライトの色を更新（黒色にアルファ値を設定）
            sprite_->SetColor({ 0,0,0,alpha });
            sprite_->Update();
        }
        /// <summary>
		/// 描画処理
        /// </summary>
        void Draw() override {
            sprite_->Draw();
        }
	private: // メンバ変数
		// フェード用スプライト
        std::unique_ptr<MyEngine::Sprite> sprite_;
    };
}