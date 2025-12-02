#pragma once
#include <Object3d.h>
#include<CameraTransform.h>
#include<CameraTypes.h>
#include<TitleCamera.h>
#include<GameCamera.h>
#include<GameOverCamera.h>

using namespace CameraTypes;

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

    // 初期化
    void Initialize(CameraTransform Transform);
    // 更新処理
    void Update();

    void SetCameraMode(CameraMode mode);

    // カメラモード切替
    void ToggleCameraMode(bool followMode); // モード切替
    void DrawImGui(); // ImGui描画   
    Vector3 cameraLerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * t;
    }

private: // メンバ変数
    ViewCameraType Typeview_;                                                  // 使用するカメラのタイプ  
    SceneCameraType activeSceneCameraType_;                                    // カメラがどこのsシーンにあるか
    CameraMode currentMode_;                                                   // カメラの状態 
    CameraSwitchType switchType_ = CameraSwitchType::Instant;                  // カメラの切替方法
    std::unique_ptr<Camera> mainCamera_;                                       //　メインカメラ(基本1つ)
    CameraTransform maintrans_;                                                // カメラの座標
    std::unordered_map<std::string, std::unique_ptr<Camera>> subCamerasMap_;   // サブカメラ(複数の設置に対応できる)
    std::string activeSubCameraName_;                                          // 登録したサブカメラの名前
    Camera* activeCamera_ = nullptr;                                           // アクティブ中のカメラ

    std::unique_ptr<SceneCameraBase> titlecamera_;
    std::unique_ptr<SceneCameraBase> gameovercamera_;
    SceneCameraBase* currentSceneCamera_;

    SceneCameraType lastSceneCameraType_ = SceneCameraType::Title;
    bool sceneCameraJustChanged_ = false;

    //// Debug用フリーカメラ
    //std::unique_ptr<Camera> debugFreeCamera_;
    //bool useDebugCamera_ = false;              // Debugカメラを使用中かどうかのフラグ
public: // メンバ関数
    Camera* GetActiveCamera();

    void RegisterSubCameras(std::vector<std::unique_ptr<Camera>>&& cameras, const std::string& prefix);

    void NotifySceneChangedByName(const std::string& sceneName);

    void OnSceneChanged(SceneCameraType type);

    void SetActiveCamera();
};