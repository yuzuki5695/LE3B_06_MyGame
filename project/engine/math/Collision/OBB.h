#pragma once
#include <Vector3.h>

/// <summary>
/// OBB（Oriented Bounding Box）構造体
/// </summary>
struct OBB {
    Vector3 center;     // 中心位置（ワールド座標）
    Vector3 halfSize;   // 各軸方向の半分サイズ（スケール/2）
    Vector3 axis[3];    // ローカル軸（回転を反映したX,Y,Z単位ベクトル）
};