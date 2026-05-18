#pragma once
#include <BaseUI.h>
#include <Sprite.h>

namespace MyGame {
    /// <summary>
	/// ゲームクリアシーンのUIクラス
    /// </summary>
    class GameClearUI : public BaseUI {
    public: // メンバ関数
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize() override;
        /// <summary>
        /// 毎フレームの更新処理
        /// </summary>
        void Update() override;
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw() override;

        bool GetMovestarted() const { return movestarted_; }
        void StartMove() { movestarted_ = true; }
		void SetMovestarted(bool value) { movestarted_ = value; }
        void StartBack() { backstarted_ = true; }
		void StartUI01Move() { ui01Started_ = true; }
		bool GetUI01Finished() const { return ui01Finished_; }
        void UI01Finished() { ui01Finished_ = false; }

    private: // メンバ変数
        std::unique_ptr<MyEngine::Sprite> mission_;
        std::unique_ptr<MyEngine::Sprite> complete_;
        std::unique_ptr<MyEngine::Sprite> ui_01_;

        // UI移動制御用の変数
        float ui2Timer_;
        float ui2Duration_; // 約1秒
        float ui3Timer_;
        float ui3Duration_;

        MyEngine::Vector2 ui2StartPos; // 上にずらした初期位置
        MyEngine::Vector2 ui2EndPos; // 最終位置（現在の値）
        MyEngine::Vector2 ui3StartPos; // 上にずらした初期位置
        MyEngine::Vector2 ui3EndPos; // 最終位置（現在の値）
        float step3TimeScale = 0.7f; // 0.3～0.6 あたりがおすすめ

        bool ui01Finished_ = false;

        float ui01Timer_ = 0.0f;
        bool movestarted_ = false; // 移動開始フラグ
        bool backstarted_ = false; // フラグ
 


        float ui01Duration_ = 60.0f;
        MyEngine::Vector2 ui01StartPos_;
        MyEngine::Vector2 ui01EndPos_;
        bool ui01Started_ = false;



        float backTimer_ = 0.0f;
float backDuration_ = 60.0f;
    };
}