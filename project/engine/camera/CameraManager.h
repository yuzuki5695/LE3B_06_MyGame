#pragma once
#include <Object3d.h>
#include <CameraTransform.h>
#include <CameraTypes.h>

namespace MyEngine {
    // カメラマネージャ
    class CameraManager {
    private:
        static std::unique_ptr<CameraManager> instance;

        CameraManager(CameraManager&) = delete;
        CameraManager& operator=(CameraManager&) = delete;
    public: // メンバ関数
        CameraManager() = default;
        ~CameraManager() = default;
        // シングルトンインスタンスの取得
        static CameraManager* GetInstance();
        // 終了
        void Finalize();
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize(CameraTransform Transform);
        /// <summary>
        /// 更新処理
        /// </summary> 
        void Update();
    private: // メンバ変数
        CameraTypes::ViewCameraType Typeview_;                                                  // 使用するカメラのタイプ  
        CameraTypes::SceneCameraType activeSceneCameraType_;                                    // カメラがどこのシーンにあるか
        CameraTypes::CameraMode currentMode_;                                                   // カメラの状態 
        CameraTypes::CameraSwitchType switchType_ = CameraTypes::CameraSwitchType::Instant;                  // カメラの切替方法
        std::unique_ptr<Camera> mainCamera_;                                       //　メインカメラ(基本1つ)
        CameraTransform maintrans_;                                                // カメラの座標
        std::unordered_map<std::string, std::unique_ptr<Camera>> subCamerasMap_;   // サブカメラ(複数の設置に対応できる)
        std::string activeSubCameraName_;                                          // 登録したサブカメラの名前
        Camera* activeCamera_ = nullptr;                                           // アクティブ中のカメラ

        // 前回のシーンカメラタイプ
        CameraTypes::SceneCameraType lastSceneCameraType_ = CameraTypes::SceneCameraType::Title;
        // シーン切替直後Update時に一度だけ各シーン用カメラの情報を反映
        bool sceneCameraJustChanged_ = false;

    public: // メンバ関数
        // 現在アクティブなカメラ
        Camera* GetActiveCamera();
    };
}