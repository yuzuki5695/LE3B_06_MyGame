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

        float LerpAngle(float a, float b, float t) {
            float diff = b - a;

            while (diff > 3.141592f)  diff -= 6.283184f;
            while (diff < -3.141592f) diff += 6.283184f;

            return a + diff * t;
        }

        void StartIntroMove();
        
        void UpdateMove(MyEngine::Camera* camera);

    private:
        MyEngine::Vector3 transform_;

        bool isIntroTargetLocked_ = false;

        float followTightness_ = 0.05f;

        float offsetX_ = 6.0f;
        float offsetY_ = -1.0f;
        float offsetZ_ = -20.0f;
    };
}