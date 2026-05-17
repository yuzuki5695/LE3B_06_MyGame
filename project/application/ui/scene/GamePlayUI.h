#pragma once
#include <BaseUI.h>
#include <Vector2.h>
#include <Pausemenu.h>

namespace MyGame {
    // 前方宣言
    class Player;

    /// <summary>
	/// ゲームプレイシーンのUIを管理するクラス
    /// </summary>
    class GamePlayUI : public BaseUI {
    public: // メンバ関数
        /// <summary>
		/// 初期化処理
        /// </summary>
        void Initialize() override;
        /// <summary>
		/// 更新処理
        /// </summary>
        void Update() override;
        /// <summary>
		/// 描画処理
        /// </summary>
        void Draw() override;
        void SetPlayer(Player* player) { player_ = player; }
    private: // プレイベートメンバ関数
        void UpdateStageProgressUI();
        // UI生成用の内部関数
        void CreateWASDUI(const MyEngine::Vector2& baseCenter, const  MyEngine::Vector2& size, float keySpacing, float groupSpacing);
        // 更新処理の細分化
        void UpdateControlUI();
        void UpdateControlUIAnimation();
    private: // メンバ変数
        std::unique_ptr<Pausemenu> pausemenu_;
        std::vector<std::unique_ptr<MyEngine::Sprite>> uis_;
        std::vector<MyEngine::Vector2> uiOriginalSizes_;
        Player* player_ = nullptr;
        bool isAnimating_;
        float timer_;
        float duration_;

        std::unique_ptr <MyEngine::Sprite> gage_;
        std::unique_ptr <MyEngine::Sprite> player_ui_;
    
        bool uiProgressStarted_ = false;
        bool uiProgressFinished_ = false;
        public: // アクセッサ
         Pausemenu* GetPauseMenu() const { return pausemenu_.get(); }
    };
}