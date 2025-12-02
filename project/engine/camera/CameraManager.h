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
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(CameraTransform Transform);
    /// <summary>
    /// 更新処理
    /// </summary> 
    void Update();
    // ImGui描画   
    void DrawImGui();
    // カメラモード設定
    void SetCameraMode(CameraMode mode); 
    // サブカメラを一括登録
    void RegisterSubCameras(std::vector<std::unique_ptr<Camera>>&& cameras, const std::string& prefix);
    // シーンマネージャーから現在のシ―ンを判定する
    void NotifySceneChangedByName(const std::string& sceneName);
    // 各シーン用カメラの切替
    void OnSceneChanged(SceneCameraType type);


private: // メンバ変数
    ViewCameraType Typeview_;                                                  // 使用するカメラのタイプ  
    SceneCameraType activeSceneCameraType_;                                    // カメラがどこのシーンにあるか
    CameraMode currentMode_;                                                   // カメラの状態 
    CameraSwitchType switchType_ = CameraSwitchType::Instant;                  // カメラの切替方法
    std::unique_ptr<Camera> mainCamera_;                                       //　メインカメラ(基本1つ)
    CameraTransform maintrans_;                                                // カメラの座標
    std::unordered_map<std::string, std::unique_ptr<Camera>> subCamerasMap_;   // サブカメラ(複数の設置に対応できる)
    std::string activeSubCameraName_;                                          // 登録したサブカメラの名前
    Camera* activeCamera_ = nullptr;                                           // アクティブ中のカメラ
    // 各シーン用カメラ
    std::unique_ptr<SceneCameraBase> titlecamera_;
    std::unique_ptr<SceneCameraBase> gameovercamera_;
    SceneCameraBase* currentSceneCamera_;
    // 前回のシーンカメラタイプ
    SceneCameraType lastSceneCameraType_ = SceneCameraType::Title;
    // シーン切替直後Update時に一度だけ各シーン用カメラの情報を反映
    bool sceneCameraJustChanged_ = false;

    //// Debug用フリーカメラ
    //std::unique_ptr<Camera> debugFreeCamera_;
    //bool useDebugCamera_ = false;              // Debugカメラを使用中かどうかのフラグ
public: // メンバ関数
    // 現在アクティブなカメラ
    Camera* GetActiveCamera();
    // アクティブカメラを更新し、共通リソースに設定する
    void SetActiveCamera();
};