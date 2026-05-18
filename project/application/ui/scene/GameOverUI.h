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
        std::unique_ptr<MyEngine::Sprite> mission_;

    };
}