#include "CollisionManager.h"
#include<MatrixVector.h>
#include <cmath>

using namespace MatrixVector;

OBB CollisionManager::GetBulletOBB(Transform transform,Vector3 scalar) const {
    OBB obb;

    // 中心は現在の位置
    obb.center = transform.translate;

    // サイズ（スケール）の半分をハーフサイズにする
    obb.halfSize = { transform.scale / scalar };

    // 軸はXYZの単位ベクトル
    obb.axis[0] = { 1,0,0 }; // X軸
    obb.axis[1] = { 0,1,0 }; // Y軸
    obb.axis[2] = { 0,0,1 }; // Z軸

    return obb;
}

bool CollisionManager::IsOBBIntersect(const OBB& a, const OBB& b) {
    const float EPSILON = 1e-5f;
    Vector3 T = b.center - a.center;

    // Aのローカル軸
    Vector3 A[3] = { a.axis[0], a.axis[1], a.axis[2] };
    // Bのローカル軸
    Vector3 B[3] = { b.axis[0], b.axis[1], b.axis[2] };

    float R[3][3];
    float AbsR[3][3];

    // 回転行列R = A軸とB軸の内積
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            R[i][j] = Dot(A[i], B[j]);
            AbsR[i][j] = std::abs(R[i][j]) + EPSILON; // 数値誤差を避ける
        }
    }

    // TをAの座標系に変換
    T = { Dot(T, A[0]), Dot(T, A[1]), Dot(T, A[2]) };

    // 15軸について判定する（SAT）
    for (int i = 0; i < 3; ++i) {
        float ra = a.halfSize[i];
        float rb = b.halfSize[0] * AbsR[i][0] + b.halfSize[1] * AbsR[i][1] + b.halfSize[2] * AbsR[i][2];
        if (std::abs(T[i]) > ra + rb) return false;
    }

    for (int i = 0; i < 3; ++i) {
        float ra = a.halfSize[0] * AbsR[0][i] + a.halfSize[1] * AbsR[1][i] + a.halfSize[2] * AbsR[2][i];
        float rb = b.halfSize[i];
        if (std::abs(T[0] * R[0][i] + T[1] * R[1][i] + T[2] * R[2][i]) > ra + rb) return false;
    }

    // 斜め軸9本のチェック（省略可能な場合もあるが安全のためやる）
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            float ra = a.halfSize[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + a.halfSize[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
            float rb = b.halfSize[(j + 1) % 3] * AbsR[i][(j + 2) % 3] + b.halfSize[(j + 2) % 3] * AbsR[i][(j + 1) % 3];
            float t = std::abs(T[(i + 2) % 3] * R[(i + 1) % 3][j] - T[(i + 1) % 3] * R[(i + 2) % 3][j]);
            if (t > ra + rb) return false;
        }
    }

    return true;
}
