//#include "RailPath.h"
//#include <algorithm>
//#include <Spline.h>
//#include <MatrixVector.h>
//
//using namespace MyEngine;
//using namespace MatrixVector;
//
//namespace MyGame {
//     
//    using namespace Math::Spline;
//
//    void RailPath::Build(const std::vector<BezierPoint>& curve) {
//        segmentLengths_.clear();
//        totalLength_ = 0.0f;
//
//        for (int i = 0; i < (int)curve.size() - 1; i++) {
//            float length = CalculateSegmentLength(curve, i);
//            segmentLengths_.push_back(length);
//            totalLength_ += length;
//        }
//    }
//
//    void RailPath::ConvertDistance(float distance, int& index, float& t) {
//        float remain = distance;
//        int segment = 0;
//
//        while (segment < (int)segmentLengths_.size()) {
//            if (remain <= segmentLengths_[segment]) break;
//            remain -= segmentLengths_[segment];
//            segment++;
//        }
//
//        segment = std::clamp(segment, 0, (int)segmentLengths_.size() - 1);
//
//        float len = segmentLengths_[segment];
//        index = segment;
//        t = (len > 0.0f) ? remain / len : 0.0f;
//    }
//
//
//    float RailPath::CalculateSegmentLength(const std::vector<BezierPoint>& curve, int index) {
//        const int div = 10;
//        float length = 0.0f;
//
//        Vector3 prev = GetPoint(curve, index, 0.0f);
//
//        for (int i = 1; i <= div; i++) {
//            float t = i / (float)div;
//            Vector3 pos = GetPoint(curve, index, t);
//            length += Length(pos - prev);
//            prev = pos;
//        }
//        return length;
//    }
//
//    Vector3 RailPath::GetPoint(const std::vector<BezierPoint>& curve, int index, float t) {
//        auto Get = [&](int i) -> const Vector3& {
//            i = std::clamp(i, 0, (int)curve.size() - 1);
//            return curve[i].controlPoint;
//            };
//
//        const Vector3& p0 = Get(index - 1);
//        const Vector3& p1 = Get(index);
//        const Vector3& p2 = Get(index + 1);
//        const Vector3& p3 = Get(index + 2);
//
//        return CatmullRom(p0, p1, p2, p3, t);
//    }
//}