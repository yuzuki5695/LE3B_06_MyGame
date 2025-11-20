#pragma once
#include <Object3d.h>
#include<GameCamera.h>
#include<CameraMode.h>
#include<CameraTransform.h>
#include<TitleCamera.h>

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
    ViewCameraType Typeview_;   // 使用するカメラのタイプ  
    SceneCameraType activeSceneCameraType_;  // シーンごとのカメラの状態
    CameraMode currentMode_; // カメラの状態 
    CameraSwitchType switchType_ = CameraSwitchType::Instant; // カメラの切替方法
    std::unique_ptr<Camera> mainCamera_;                                   //　メインカメラ(基本1つ)
    std::unordered_map<std::string, std::unique_ptr<Camera>> subCamerasMap_;   // サブカメラ(複数の設置に対応できる)
    Camera* activeCamera_ = nullptr;
    std::unique_ptr<TitleCamera> titlecamera_;

public: // メンバ関数
        
    Camera* GetActiveCamera();


    void SetSceneCameraType(SceneCameraType type) {
        activeSceneCameraType_ = type;
    }

    SceneCameraType GetSceneCameraType() const {
        return activeSceneCameraType_;
    }

    SceneCameraType ConvertSceneNameToCameraType(const std::string& name) {
        if (name == "TITLE")     return SceneCameraType::Title;
        if (name == "GAMEPLAY")  return SceneCameraType::Gameplay;
        if (name == "GAMECLEAR") return SceneCameraType::GameClear;
        if (name == "GAMEOVER")  return SceneCameraType::GameOver;

        return SceneCameraType::Title; // デフォルト
    }

    void SetActiveCamera();

};