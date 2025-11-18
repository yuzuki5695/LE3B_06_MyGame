#pragma once
#include <memory>
#include <Camera.h>
#include <Object3d.h>

class ClearCamera{
public:
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

private: // メンバ変数
    std::unique_ptr<Camera> maincamera_ = nullptr;  // 実際に描画で使用されるカメラインスタンス  

    // 追従対象
    Object3d* target_ = nullptr;

    // 最初のオフセットを保存するフラグ
    bool addedInitialOffset_ = false;
    Vector3 offset_; // ターゲットとの相対位置

public: 
    // 追従ターゲットをセット
    void SetTarget(Object3d* target) { target_ = target; }
  
    Camera* GetActiveCamera() const { return maincamera_.get(); }
};