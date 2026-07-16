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
        /// ターゲットを注視する
        /// </summary>
        /// <param name="camera"></param>
        void UpdateLookAt(MyEngine::Camera* camera);
        /// <summary>
        /// タイトル演出用カメラ移動を開始する
        /// </summary>
        void StartIntroMove();
    private: // 内部関数
        /// <summary>
        /// タイトル演出用カメラ移動を更新する
        /// </summary>
        /// <param name="camera"></param>
        void UpdateMove(MyEngine::Camera* camera);    
    private: // メンバ変数
        // カメラの目標位置
        MyEngine::Vector3 targetPosition_;
        MyEngine::Vector3 kInitialposition_;       // カメラ初期位置
        MyEngine::Vector3 kOffset_;                // ターゲットからのオフセット
        float kfollowtightness_;                   // カメラ移動補間係数
        float krotationlerp_;                      // カメラ回転補間係数
        float kmindirectionlength_;                // 正規化を行う最小ベクトル長
        float kmovecompletedistance_;              // 移動完了判定距離
        bool isIntrotargetlocked_;                 // 演出移動の目標位置を固定済みか
    };
}