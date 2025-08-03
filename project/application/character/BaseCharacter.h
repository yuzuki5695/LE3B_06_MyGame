#pragma once
#include <memory>
#include <Object3d.h>

struct OBB {
    Vector3 center;     // 中心位置
    Vector3 halfSize;   // 各軸方向の半径（スケールの半分）
    Vector3 axis[3];    // ローカル軸（回転を反映させたX, Y, Zベクトル）
};

class BaseCharacter {
public:
    virtual ~BaseCharacter() = default;

    // 基本処理
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
};