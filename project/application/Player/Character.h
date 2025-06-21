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
};