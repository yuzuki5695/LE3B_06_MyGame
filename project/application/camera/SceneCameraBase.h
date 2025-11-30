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
    const CameraTransform& GetMainTransform() const { return transform_; }
    /// サブカメラの追加
    virtual void AddSubCamera(const CameraTransform& trans) {} 
    /// サブカメラの追加（複数登録に対応）
    virtual void AddSubCameras(const std::vector<CameraTransform>& transforms) {} 
    /// サブカメラ生成（CameraManager に所有権を渡す）
    virtual std::vector<std::unique_ptr<Camera>> MoveSubCameras() { return std::move(subcameras_); }

    /// サブカメラをコピーして取得（CameraManager の初期化用）
    virtual std::vector<std::shared_ptr<Camera>> GetSubCameras() const {
        std::vector<std::shared_ptr<Camera>> result;
        for (const auto& cam : subcameras_) {
            result.push_back(std::make_shared<Camera>(*cam)); // Camera のコピーコンストラクタを使用
        }
        return result;
    }
protected:
    CameraTransform transform_;                       // メインカメラ用の位置・回転
    std::vector<std::unique_ptr<Camera>> subcameras_; // サブカメラ群(デフォルトは空)
};