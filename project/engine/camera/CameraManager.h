#pragma once
#include <Object3d.h>
#include<GameCamera.h>
// カメラモード
enum class CameraMode {
    Default,
    Follow,
    GamePlay,
    Event
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
	// カメラモード切替
    void ToggleCameraMode(bool followMode); // モード切替
    void DrawImGui(); // ImGui描画   
    void SetActiveCamera(); // アクティブカメラを設定

private:

    std::unique_ptr<Camera> mainCamera_;                 //　メインカメラ
    std::vector<std::unique_ptr<Camera>> subCameras_;    // サブカメラ
    std::unordered_map<std::string, std::unique_ptr<Camera>> cameraMap_;    // サブカメラを複数使用する場合の登録用カメラ

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