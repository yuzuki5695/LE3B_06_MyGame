#pragma once
#include <memory>
#include <IFadeEffect.h>

namespace MyGame {
    /// <summary>
    /// フェードを管理するマネージャークラス
    /// </summary>
    class FadeManager {
    private:
        static std::unique_ptr<FadeManager> instance;

        FadeManager(FadeManager&) = delete;
        FadeManager& operator=(FadeManager&) = delete;
    public: // メンバ関数
        FadeManager() = default;
        ~FadeManager() = default;
        // シングルトンインスタンスの取得
        static FadeManager* GetInstance();
        /// <summary>
        /// 終了処理
        /// </summary>
        void Finalize();
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize();
        /// <summary>
        /// 更新処理
        /// </summary>
        void Update();
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw();

        void StartFade(FadeType type, FadeStyle style, float duration);

    private: // メンバ変数
        std::unique_ptr<IFadeEffect> effect_;

        FadeType type_ = FadeType::None;
        float timer_ = 0.0f;
        float duration_ = 1.0f;
        float t_ = 0.0f;
        bool isFading_ = false;

    };
}