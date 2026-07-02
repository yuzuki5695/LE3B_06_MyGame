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

        void ShowExpBar();
        void ShowLevelUp(bool isMaxLevel);
        /// <summary>
        /// Playerを取得後に呼び出すことで、HPUIの初期化を行う
        /// </summary>
        void SetPlayerHp();
    private: // プレイベートメンバ関数
        void UpdateStageProgressUI();
        // UI生成用の内部関数
        void CreateWASDUI(const MyEngine::Vector2& baseCenter, const  MyEngine::Vector2& size, float keySpacing, float groupSpacing);
        // 更新処理の細分化
        void UpdateControlUI();
        // UIアニメーションの更新
        void UpdateControlUIAnimation();
        // プレイヤーの位置に追従するUIの更新
        void UpdatePlayerFollowUI();
        // EXPバーのフェード更新
        void UpdateExpBarFade();
        // レベルアップUIの更新
        void UpdateLevelUpFade();
    private: // メンバ変数
        Player* player_ = nullptr; // プレイヤーへの参照
        std::unique_ptr<Pausemenu> pausemenu_; // ポーズメニュー
        std::vector<std::unique_ptr<MyEngine::Sprite>> uis_; // 操作UIのスプライトリスト
        std::unique_ptr <MyEngine::Sprite> gage_;       // ステージ進行度を示すUI
        std::unique_ptr <MyEngine::Sprite> player_ui_;  // プレイヤー位置を示すUI
        std::vector<MyEngine::Vector2> uiOriginalSizes_; // 操作UIの元のサイズを保存するリスト
        bool isAnimating_;   // 操作UIのアニメーション中かどうかのフラグ
        float timer_;  // 操作UIのアニメーション開始からの経過時間
        float duration_;  // 操作UIのアニメーションにかける時間

        bool uiProgressStarted_ = false;  // ステージ進行度UIのアニメーションが開始したかどうかのフラグ
        bool uiProgressFinished_ = false; // ステージ進行度UIのアニメーションが完了したかどうかのフラグ
        bool isEventLocked;

        // 経験値関連
        std::unique_ptr<MyEngine::Sprite> expBarBack_;
        std::unique_ptr<MyEngine::Sprite> expBarFill_;
        bool isExpBarVisible_;   // 表示状態
        float expBarAlpha_;     // フェード
        float expBarTimer_;     // 表示タイマー
        float expBarDuration_;     // 表示時間
        bool isFirstExpBar_ = true;
        // レベルアップ通知用UI
        std::unique_ptr<MyEngine::Sprite> levelup_;
        MyEngine::Vector2 levelUpBasePos_;
        bool isLevelUpVisible_;
        float levelUpAlpha_;
        float levelUpTimer_;
        // --- HP表示用の変数 ---
        std::vector<std::unique_ptr<MyEngine::Sprite>> hpHearts_; // HPアイコンのリスト
        const float kHeartStartX = 100.0f; // 表示開始のX座標
        const float kHeartY = 15.0f;       // 表示のY座標
        const float kHeartOffsetX = 75.0f;// X軸のずらし幅
        const MyEngine::Vector2 kHeartSize = { 64.0f, 64.0f }; // アイコンサイズ
    public: // アクセッサ
        Pausemenu* GetPauseMenu() const { return pausemenu_.get(); }
        void SetPlayer(Player* player) { player_ = player; }
        void SetEventLocked(bool flag) { isEventLocked = flag; }
        void SetExpBarVisible(bool flag) { isExpBarVisible_ = flag; }
    };
}