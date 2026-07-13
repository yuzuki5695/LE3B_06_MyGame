#pragma once
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>

namespace MyEngine {
    namespace MathUtil {
        /// <summary>
        /// 球面線形補間（Slerp）
        /// </summary>
        /// <param name="start">開始ベクトル</param>
        /// <param name="end">終了ベクトル</param>
        /// <param name="t">補間係数（0.0～1.0）</param>
        /// <returns>補間後のベクトル</returns>
        Vector3 Slerp(const Vector3& start, const Vector3& end, float t);

        /// <summary>
        /// 前方向ベクトルから回転角を求める
        /// </summary>
        /// <param name="forward">前方向ベクトル</param>
        /// <returns>回転角（Pitch・Yaw・Roll）</returns>
        Vector3 LookAtRotation(const Vector3& forward);

        /// <summary>
        /// 座標を行列変換する
        /// </summary>
        /// <param name="vector">変換する座標</param>
        /// <param name="matrix">変換行列</param>
        /// <returns>変換後の座標</returns>
        Vector4 TransformCoord(const Vector4& vector, const Matrix4x4& matrix);

        /// <summary>
        /// 角度を最短経路で線形補間する
        /// </summary>
        /// <param name="startAngle">開始角度（ラジアン）</param>
        /// <param name="endAngle">終了角度（ラジアン）</param>
        /// <param name="t">補間係数（0.0～1.0）</param>
        /// <returns>補間後の角度</returns>
        float LerpAngle(float startAngle, float endAngle, float t);
    }
}