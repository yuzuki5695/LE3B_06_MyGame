#pragma once
#include <memory>
#include <Object3d.h>

class BaseCharacter {
public:
    virtual ~BaseCharacter() = default;

    // 基本処理
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
};