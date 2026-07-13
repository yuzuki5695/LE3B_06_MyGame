#pragma once
#include <ISceneCameraBehavior.h>

namespace MyGame {  
    /// <summary>
	/// クリアシーン用カメラクラス
    /// </summary>
    class GameClearCamera : public ISceneCameraBehavior {
    public: // メンバ関数
        /// <summary>
        /// 初期化処理
        /// </summary>
        /// <param name="camera"></param>
        void Initialize(MyEngine::Camera* camera) override;
        /// <summary
        /// 更新処理 
        /// </summary>
        /// <param name="camera"></param>
        void Update(MyEngine::Camera* camera) override;
    private: // 内部関数
        /// <summary>
        /// ターゲットへ向くようカメラの回転を更新する
        /// </summary>
        /// <param name="camera"></param>
        void UpdateLookAt(MyEngine::Camera* camera);
    private: // 定数
        static constexpr float kMoveDistance = 8.0f;          // カメラの移動距離
        static constexpr float kMoveSpeed = 0.02f;            // 移動補間速度
        static constexpr float kRotationLerp = 0.1f;          // 回転補間係数
        static constexpr float kMinDirectionLength = 0.0001f; // 正規化を行う最小ベクトル長
    private: // メンバ変数
        // カメラの位置
        MyEngine::Vector3 transform_;
        // カメラ移動用
        float moveStartX_;      // カメラ移動の開始位置
        float moveEndX_;        // カメラ移動の終了位置
        float moveTimer_;       // 移動補間用タイマー
        float moveEase_;        // イージング後の補間係数
        // 注視初期化フラグ
        bool isLookAtInitialized_;
    };
}