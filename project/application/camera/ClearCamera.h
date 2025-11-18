#pragma once
#include <memory>
#include <Camera.h>

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
};

