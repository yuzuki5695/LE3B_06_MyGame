#pragma once
#include <BaseScene.h>
#include <Player.h>
#include <SoundLoader.h>

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

        void PlayerMotion();

    private: // メンバ変数
        std::unique_ptr <Player> player_;
        MyEngine::Vector3 playeroffset_;
        bool isMoving_ = true;
        float movetimer_ = 0.0f;
        float moveDuration_ = 2.0f; // 秒
        float startZ_ = -100.0f;
        float targetZ_ = 100.0f;
        float floatTime_ = 0.0f;
        float floatSpeed_ = 2.0f;
        float floatAmplitude_ = 2.0f;
        bool isCameraTriggered_ = false;
        // 決定音
        MyEngine::SoundData button_; 
    };
}