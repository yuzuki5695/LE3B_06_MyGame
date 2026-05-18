#pragma once
#include <memory>
#include <IFadeEffect.h>

namespace MyGame {
    /// <summary>
    /// フェードを管理するマネージャークラス
    /// </summary>
    class FadeManager {
	private: // シングルトンインスタンス
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
        /// 更新処理
        /// </summary>
        void Update();
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw();
        /// <summary>
		/// フェード開始
        /// </summary>
        /// <param name="type"></param>
        /// <param name="style"></param>
        /// <param name="duration"></param>
        void StartFade(FadeType type, FadeStyle style, float duration);       
        /// <summary>
		/// フェード開始後にシーン切り替えを予約
        /// </summary>
        /// <param name="sceneName"></param>
        /// <param name="style"></param>
        /// <param name="duration"></param>
        void SceneChangeFade(const std::string& sceneName, FadeStyle style, float duration);
    private: // メンバ変数
        // 現在のフェードエフェクト
        std::unique_ptr<IFadeEffect> effect_;
        FadeType type_ = FadeType::None;
        float timer_ = 0.0f;
        float duration_ = 1.0f;
        float t_ = 0.0f;
        bool isFading_ = false;
        // シーン切り替え予約用変数
        std::string nextSceneName_;
        bool isSceneChangeReserved_ = false; // フェード完了後にシーン切り替えを行うか
        bool isFadeEnd_ = false;         // フェード終了済み
        bool isFadeInFinished_ = false;  // フェードイン完了
    public:
        bool IsFading() const { return isFading_; }
        bool IsFadeEnd() const { return isFadeEnd_; }
        bool IsFadeInFinished() const { return isFadeInFinished_; }
		bool IsSceneChangeReserved() const { return isSceneChangeReserved_; }
    };
}