#pragma once
#include<SceneCameraBase.h>

// ゲームオーバー専用カメラ
class GameClearCamera : public SceneCameraBase {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 更新処理
    /// </summary> 
    void Update() override;

    // サブカメラの追加
    void AddSubCamera(const CameraTransform& trans) override;
    // サブカメラの追加（複数登録に対応）
    void AddSubCameras(const std::vector<CameraTransform>& transforms) override;
};