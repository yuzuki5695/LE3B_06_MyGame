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

    private:
        MyEngine::Vector3 transform_ = { 0.0f, 0.0f, -100.0f };
        float startX = -15.0f;
        float startX2 = 0.0f;
        float endX = 0.0f;
        float t = 0.0f;
		float easeT = 0.0f;

		bool lookAtInitialized_ = false; // 注視状態の初期化フラグ

    };
}