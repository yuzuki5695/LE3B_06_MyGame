#pragma once
#include <BaseUI.h>
#include <Sprite.h>

namespace MyGame {
    /// <summary>
	/// ゲームオーバーシーンのUIクラス
    /// </summary>
    class GameOverUI : public BaseUI {
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
    private: // メンバ変数
        std::unique_ptr<MyEngine::Sprite> gameover_;
        std::unique_ptr<MyEngine::Sprite> enter_;
        MyEngine::Vector2 ui1StartPos; // 上にずらした初期位置
        MyEngine::Vector2 ui1EndPos; // 最終位置（現在の値）
        MyEngine::Vector2 ui2StartPos;
        MyEngine::Vector2 ui2EndPos;

        float ui1Timer_;
        float ui1Duration_;
        float ui2Timer_;
        float ui2Duration_;

        uint32_t phase_;
    
    };
}