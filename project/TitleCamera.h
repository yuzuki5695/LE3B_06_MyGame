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
    void Update();

    std::unique_ptr<Camera> maincamera_ = nullptr;  // 実際に描画で使用されるカメラインスタンス  
    std::unique_ptr<Camera> subcamera_ = nullptr; // サブカメラ用インスタンス
};

