#pragma once
#include <Object3d.h>
#include<GameCamera.h>
#include<CameraMode.h>
#include<CameraTransform.h>

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

    void SetViewType(ViewCameraType type, CameraSwitchType switchType);

    // カメラモード切替
    void ToggleCameraMode(bool followMode); // モード切替
    void DrawImGui(); // ImGui描画   
    void SetActiveCamera(); // アクティブカメラを設定

    Vector3 cameraLerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * t;
    }

private: // メンバ変数
    ViewCameraType Typeview_;   // 使用するカメラのタイプ  
    CameraMode currentMode_; // カメラの状態 
    CameraSwitchType switchType_ = CameraSwitchType::Instant; // カメラの切替方法
    std::unique_ptr<Camera> mainCamera_;                                   //　メインカメラ(基本1つ)
    std::unordered_map<std::string, std::unique_ptr<Camera>> subCamerasMap_;   // サブカメラ(複数の設置に対応できる)



    // イージング用
    Vector3 startPos_;
    Vector3 targetPos_;
    Vector3 startRot_;
    Vector3 targetRot_;
    float easeTimer_ = 0.0f;
    float easeTime_ = 5.0f;
    bool easing_ = false;
    Camera* targetCamera_ = nullptr;
    Camera tempCamera_; // 補間用カメラ
    float deltaTime_ = 0.016f; // 仮初期値(60fps)

public: // メンバ関数
    //// 追従対象をセット（nullptrなら追従なし）
    //void SetTarget(Object3d* target);
    //Camera* GetFollowCamera() { return followCamera_; } // 追従カメラ取得
    Camera* GetActiveCamera();

    //void SetCameraMode(CameraMode mode);
    //
    //CameraMode GetcurrentMode() { return currentMode_; }
    //GameCamera* GetGameCamera() { return gameCamera_; }


    //void SetMoveFlag(bool flag) { moveFlag = flag; }
    //bool GetMoveFlag() const { return moveFlag; }
};