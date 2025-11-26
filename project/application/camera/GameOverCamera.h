#pragma once
#include <Camera.h>
#include <memory>
#include<CameraTransform.h>
#include <vector>

// ゲームオーバー専用カメラ
class GameOverCamera
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

private: // メンバ変数
    CameraTransform maintransform_;

public: // アクセッサ（Getter / Setter）
    const CameraTransform& GetMainTransform() const { return maintransform_; }
};