#pragma once
#include <BaseScene.h>
#include <Player.h>

namespace MyGame {
    /// <summary>
    /// ゲームクリアシーン
    /// </summary>
    class GameClearScene : public MyEngine::BaseScene {
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
        /// 毎フレーム更新処理
        /// </summary>
        void Update() override;
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw() override;

        // クリア演出を段階的に行う
        void UpdateStep();
        // 各演出段階
        void Step1_MovePlayerAndSwitchCamera();
        void Step2_WaitOrDoSomething();
        void Step3_MoveCameraOnInput();

        bool EaseMove(MyEngine::Vector3& current, const MyEngine::Vector3& start, const MyEngine::Vector3& end, float& t, float speed);

    private: // メンバ変数
        std::unique_ptr <Player> player_; // プレイヤークラス
        MyEngine::Vector3 startOffset_;
        MyEngine::Vector3 endOffset_;
        MyEngine::Transform offset_;

        uint32_t step_ = 0;
        float easeT_ = 0.0f;  // 0.0 → 1.0 まで進めていく
        float easeSpeed_ = 0.01f; // イージング速度
        // Step1 カメラ
        float step1CamT_ = 0.0f;
        bool step1CamMoveStart_ = false;

        float step2CamT_ = 0.0f;   // カメラX移動用のt
        float step2Time_ = 0.0f;   // 上下揺れ用の時間
        // GameClearScene.h にフラグ追加
        bool step2FinishPlayerEase_ = false;
    };
}