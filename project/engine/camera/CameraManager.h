#pragma once
#include <Object3d.h>
#include<GameCamera.h>

enum class CameraMode {
    Default,
    Follow,
    GamePlay,
};

struct CameraTransform {
    Vector3 translate; // カメラの位置
    Vector3 rotate; // カメラの回転
};

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

    void ToggleCameraMode(bool followMode); // モード切替
    void DrawImGui(); // ImGui描画

    void SetActiveCamera(); // アクティブカメラを設定

private:
    // 現在のカメラモード
    CameraMode currentMode_;
    Object3d* target_ = nullptr; // 追従対象オブジェクト

    Camera* defaultCamera_; // 追従しないカメラ(デフォルト)
    Camera* followCamera_;  // 追従用カメラ
    GameCamera* gameCamera_;

    // ヘッダーかクラス内に追加
    bool addedInitialOffset_ = false;

    bool useFollowCamera_ = false; // カメラモード切替用フラグ
    float waitTime_ = 0.0f;
    bool activated_ = false;    
    bool moveFlag = false;

public: // メンバ関数
    // 追従対象をセット（nullptrなら追従なし）
    void SetTarget(Object3d* target);
    Camera* GetFollowCamera() { return followCamera_; } // 追従カメラ取得
    Camera* GetActiveCamera();
    void SetCameraMode(CameraMode mode);

    CameraMode GetcurrentMode() { return currentMode_; }
    GameCamera* GetGameCamera() { return gameCamera_; }
};