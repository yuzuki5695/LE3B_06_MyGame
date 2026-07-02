#pragma once
#include <ISceneCameraBehavior.h>

namespace MyGame {
    /// <summary>
	/// ゲームオーバー用カメラクラス
    /// </summary>
    class GameOverCamera : public ISceneCameraBehavior {
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
	private: // メンバ変数
		// カメラ座標
        MyEngine::Vector3 translate_;
    };
}