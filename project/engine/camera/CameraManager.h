#pragma once
#include <Object3d.h>
#include<CameraTransform.h>
#include<CameraTypes.h>
#include<TitleCamera.h>
#include<GamePlayCamera.h>
#include<GameOverCamera.h>
#include<GameClearCamera.h>

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
    // 各シーン用カメラクラス
    std::unique_ptr<TitleCamera> title_;      // タイトル
    std::unique_ptr<GamePlayCamera> gameplay_;        // ゲームプレイ
    std::unique_ptr<GameClearCamera> gameclear_;  // ゲームオーバー
    std::unique_ptr<GameOverCamera> gameover_;    // ゲームクリア


    SceneCameraBase* currentSceneCamera_;
    // 前回のシーンカメラタイプ
    SceneCameraType lastSceneCameraType_ = SceneCameraType::Title;
    // シーン切替直後Update時に一度だけ各シーン用カメラの情報を反映
    bool sceneCameraJustChanged_ = false;

public: // メンバ関数
    // 現在アクティブなカメラ
    Camera* GetActiveCamera();
    // アクティブカメラを更新し、共通リソースに設定する
    void SetActiveCamera();

    ViewCameraType GetTypeview() const { return Typeview_; }
    void SetTypeview(ViewCameraType type) { Typeview_ = type; }
    CameraMode GetMode() const { return currentMode_; }
    void SetMode(CameraMode mode) { currentMode_ = mode; }
    GamePlayCamera* GetGameCamera() const { return gameplay_.get(); };
    // ゲーム用カメラ（GameCamera）を返す getter
    GamePlayCamera* GetGameplayCamera() const { return gameplay_.get(); }
    GameClearCamera* GetGameClearCamera() { return gameclear_.get(); }
    TitleCamera* GetTitleCamera() { return title_.get(); }

    SceneCameraType GetActiveSceneCamera() const{ return activeSceneCameraType_; }

    CameraSwitchType GetSwitchType() const { return switchType_; }
    void SetSwitchType(CameraSwitchType type) { switchType_ = type; }

    Camera* GetMainCamera() const {
        return mainCamera_.get();
    }

    const CameraTransform& GetMainCameraTransform() const {
        return maintrans_;
    }

    Camera* GetSubCamera(const std::string& name) {
        auto it = subCamerasMap_.find(name);
        if (it != subCamerasMap_.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const std::unordered_map<std::string, std::unique_ptr<Camera>>& GetSubCameraMap() const {
        return subCamerasMap_;
    }

    void SetGamecameraTarget(Object3d* target) {
        if (!gameplay_) return;
        gameplay_->SetFollowTarget(target);
    }
};