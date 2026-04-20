#include "RailSampler.h"
#include <algorithm>
#include <Spline.h>

using namespace MyEngine;

namespace MyGame {

    using namespace Math::Spline;

    void RailSampler::SetCurve(const std::vector<BezierPoint>& curve) {
        curve_ = curve;
    }

    Vector3 RailSampler::Sample(int index, float t) {
        auto GetPoint = [&](int i) -> const Vector3& {
            i = std::clamp(i, 0, (int)curve_.size() - 1);
            return curve_[i].controlPoint;
            };

        const Vector3& p0 = GetPoint(index - 1);
        const Vector3& p1 = GetPoint(index);
        const Vector3& p2 = GetPoint(index + 1);
        const Vector3& p3 = GetPoint(index + 2);

        return CatmullRom(p0, p1, p2, p3, t);
    }

    Vector3 RailSampler::SampleTangent(int index, float t) {
        auto GetPoint = [&](int i) -> const Vector3& {
            i = std::clamp(i, 0, (int)curve_.size() - 1);
            return curve_[i].controlPoint;
            };

        const Vector3& p0 = GetPoint(index - 1);
        const Vector3& p1 = GetPoint(index);
        const Vector3& p2 = GetPoint(index + 1);
        const Vector3& p3 = GetPoint(index + 2);

        return CatmullRomTangent(p0, p1, p2, p3, t);
    }
}