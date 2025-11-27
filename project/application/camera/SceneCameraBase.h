#pragma once
#include <CameraTransform.h>
#include <Camera.h>
#include <vector>
#include <memory>

// シーンごとのカメラ基底クラス
class SceneCameraBase {
public:  
    virtual ~SceneCameraBase() = default;

    /// <summary>
    /// 初期化処理（シーンごとに異なるので純粋仮想）
    /// </summary>
    virtual void Initialize() = 0;

    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    virtual void Update() = 0;

    /// メインカメラ Transform の取得
    const CameraTransform& GetMainTransform() const { return maintransform_; }
    /// サブカメラの追加
    virtual void AddSubCamera(const CameraTransform& trans) {} 
    // サブカメラの追加（複数登録に対応）
    virtual void AddSubCameras(const std::vector<CameraTransform>& transforms) {} 
    // サブカメラ生成
    virtual std::vector<std::shared_ptr<Camera>> MoveSubCameras() { return std::move(subcameras_); }
    virtual std::vector<std::shared_ptr<Camera>> GetSubCameras() const { return subcameras_; }
protected:
    CameraTransform maintransform_;
    std::vector<std::shared_ptr<Camera>> subcameras_; // デフォルトは空
};