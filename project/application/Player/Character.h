#pragma once
#include <memory>
#include <Object3d.h>

class Character {
public:
    virtual ~Character() = default;

    // 基本処理
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    // 共通的な取得関数
    //virtual Object3d* GetObject3d() = 0;

 
    virtual Transform GetTransform() const = 0;
};