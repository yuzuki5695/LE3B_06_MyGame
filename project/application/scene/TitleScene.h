#pragma once
#include <BaseScene.h>

namespace MyGame {
    /// <summary>
    /// タイトルシーン
    /// タイトル画面を管理するシーン。
    /// UI 表示、タイトル演出用モデル、パーティクル、フェード制御を担当する。
    /// </summary>
    class TitleScene : public MyEngine::BaseScene {
    public: // メンバ関数
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize() override;
        /// <summary>
        /// 終了処理
        /// </summary>
        void Finalize() override;
        /// <summary>
        /// 毎フレームの更新処理
        /// </summary>
        void Update() override;
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw() override;
    private: // メンバ変数
    };
}