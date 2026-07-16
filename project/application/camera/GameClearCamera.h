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
    private: // メンバ変数
        // カメラの位置
        MyEngine::Vector3 transform_;
        // カメラ移動用
        float movestartx_;           // カメラ移動の開始位置
        float moveendx_;             // カメラ移動の終了位置
        float movetimer_;            // 移動補間用タイマー
        float moveease_;             // イージング後の補間係数
        bool islookatInitialized_;   // 注視初期化フラグ
        float kmovedistance_;        // カメラの移動距離
        float kmovespeed_;           // 移動補間速度
        float krotationlerp_;        // 回転補間係数
        float kmindirectionlength_;  // 正規化を行う最小ベクトル長
    };
}