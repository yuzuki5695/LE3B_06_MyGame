#pragma once
#include <ISceneCameraBehavior.h>

namespace MyGame {
    /// <summary>
	/// タイトルシーン用カメラクラス
    /// </summary>
    class TitleCamera : public ISceneCameraBehavior {
	public: // メンバ関数
        /// <summary>
		/// 初期化処理
        /// </summary>
        /// <param name="camera"></param>
        void Initialize(MyEngine::Camera* camera)override;
        /// <summary>
		/// 更新処理 
        /// </summary>
        /// <param name="camera"></param>
        void Update(MyEngine::Camera* camera) override;
        /// <summary>
		/// 注視状態の更新
        /// </summary>
        /// <param name="camera"></param>
        void UpdateLookAt(MyEngine::Camera* camera);
    private: // 内部関数
        /// <summary>
        /// ターゲットを注視するようカメラの回転を更新する
        /// </summary>
        /// <param name="camera"></param>
        void UpdateLookAt(MyEngine::Camera* camera);
        /// <summary>
        /// タイトル演出用カメラ移動を開始する
        /// </summary>
        void StartIntroMove();
        /// <summary>
        /// タイトル演出用カメラ移動を更新する
        /// </summary>
        /// <param name="camera"></param>
        void UpdateMove(MyEngine::Camera* camera);
    private: // 定数
        // カメラ初期位置
        static constexpr MyEngine::Vector3 kInitialPosition = { 5.0f, -3.0f, 0.0f };
        // ターゲットからのオフセット
        static constexpr float kOffsetX = 6.0f;
        static constexpr float kOffsetY = -1.0f;
        static constexpr float kOffsetZ = -20.0f;
        // カメラ移動補間係数
        static constexpr float kFollowTightness = 0.05f;
        // カメラ回転補間係数
        static constexpr float kRotationLerp = 0.1f;
        // 正規化を行う最小ベクトル長
        static constexpr float kMinDirectionLength = 0.0001f;
        // 移動完了判定距離
        static constexpr float kMoveCompleteDistance = 0.1f;
    private: // メンバ変数
        // カメラの目標位置
        MyEngine::Vector3 targetPosition_;
        // 演出移動の目標位置を固定済みか
        bool isIntroTargetLocked_;
    };
}