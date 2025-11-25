#pragma once
#include <Camera.h>
#include <memory>
#include<CameraTransform.h>
#include <vector>

// タイトル専用カメラ
class TitleCamera
{
public: // メンバs関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary> 
    void Update();

    // サブカメラ情報の取得
    std::vector<std::unique_ptr<Camera>>& GetSubCameras() { return subCameras_; }
    // サブカメラ追加
    void AddSubCamera(const CameraTransform& trans);
    void AddSubCameras(const std::vector<CameraTransform>& transforms);


private: // メンバ変数
    CameraTransform maintransform_;
    std::vector<std::unique_ptr<Camera>> subCameras_;


public: // アクセッサ（Getter / Setter）
    const CameraTransform& GetMainTransform() const { return maintransform_; }
};