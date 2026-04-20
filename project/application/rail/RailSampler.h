#pragma once
#include <CurveJsonLoader.h>

namespace MyGame {
    /// <summary>
    /// レール上の位置と接線をサンプリングするクラス
    /// </summary>
    class RailSampler {
    public: // メンバ関数
        /// <summary>
        /// ベジェ曲線の制御点をセットする
        /// </summary>
        /// <param name="curve"></param>
        void SetCurve(const std::vector<BezierPoint>& curve);
        /// <summary>
        /// 指定した区間と進行度に基づいて、レール上の位置をサンプリングする
        /// </summary>
        /// <param name="index"></param>
        /// <param name="t"></param>
        /// <returns></returns>
        MyEngine::Vector3 Sample(int index, float t);
        /// <summary>
        /// 指定した区間と進行度に基づいて、レール上の接線をサンプリングする
        /// </summary>
        /// <param name="index"></param>
        /// <param name="t"></param>
        /// <returns></returns>
        MyEngine::Vector3 SampleTangent(int index, float t);
    private: // メンバ変数
        std::vector<BezierPoint> curve_;
    };
}