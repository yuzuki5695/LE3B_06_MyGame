#pragma once
#include <Vector3.h>

// カメラの基本情報
struct CameraTransform {
    Vector3 translate; // カメラ位置
    Vector3 rotate;    // カメラ回転
};