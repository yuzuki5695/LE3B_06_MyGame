#pragma once
#include<SceneCameraBase.h>
#include <Object3d.h>

// ゲームクリア専用カメラ
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

    // --- 追従対象を設定 ---
    void SetTarget(Object3d* target) { target_ = target; }


private: // メンバ変数
   
    // 追従対象
    Object3d* target_ = nullptr;

    float followDistance_ = -20.0f; // 後ろから見る距離（調整可）

};