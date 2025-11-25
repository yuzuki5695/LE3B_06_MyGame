#pragma once
#include <Object3d.h>
#include<CameraTransform.h>
#include<CameraTypes.h>
#include<TitleCamera.h>
#include<GameCamera.h>

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

    // 各シーンのサブカメラの登録
    void RegisterSubCameras(std::vector<std::unique_ptr<Camera>>& cameras, const std::string& prefix);

private: // メンバ変数
    ViewCameraType Typeview_;                                                  // 使用するカメラのタイプ  
    SceneCameraType activeSceneCameraType_;                                    // シーンごとのカメラの状態
    CameraMode currentMode_;                                                   // カメラの状態 
    CameraSwitchType switchType_ = CameraSwitchType::Instant;                  // カメラの切替方法
    std::unique_ptr<Camera> mainCamera_;                                       //　メインカメラ(基本1つ)
    CameraTransform maintrans_;                                                // カメラの座標
    std::unordered_map<std::string, std::unique_ptr<Camera>> subCamerasMap_;   // サブカメラ(複数の設置に対応できる)
    std::string activeSubCameraName_;                                          // 登録したサブカメラの名前
    Camera* activeCamera_ = nullptr;                                           // アクティブ中のカメラ
    // タイトル専用カメラ
    std::unique_ptr<TitleCamera> titlecamera_;




    //// Debug用フリーカメラ
    //std::unique_ptr<Camera> debugFreeCamera_;
    //bool useDebugCamera_ = false;              // Debugカメラを使用中かどうかのフラグ
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