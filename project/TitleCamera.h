#pragma once
#include <Camera.h>
#include <memory>

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
    void Update(float dt);
    
    // イージング開始
    void StartEaseToMain();
    void StartEaseToSub();

private: // メンバ変数
    std::unique_ptr<Camera> maincamera_ = nullptr;  // 実際に描画で使用されるカメラインスタンス  
    std::unique_ptr<Camera> subcamera_ = nullptr; // サブカメラ用インスタンス

    // --- イージング制御用 ---
    bool easing_ = false;
    float easeTimer_ = 0.0f;
    float easeTime_  = 2.0f;

    Vector3 startPos_;
    Vector3 startRot_;
    Vector3 targetPos_;
    Vector3 targetRot_;

public: // アクセッサ（Getter / Setter）

    Camera* GetMainCamera()  { return maincamera_.get(); }
    Camera* GetSubCamera()   { return subcamera_.get(); }

};