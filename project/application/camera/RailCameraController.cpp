//#include "RailCameraController.h"
//#include <MathUtil.h>
//#include <MatrixVector.h>
//
//using namespace MyEngine;
//using namespace MatrixVector;
//using namespace MathUtil;
//
//namespace MyGame {
//    void RailCameraController::Initialize(const std::vector<BezierPoint>& points) {
//        points_ = points;
//
//        currentSegment_ = 0;
//        t_ = 0.0f;
//        moving_ = true;
//
//        position_ = points_[0].controlPoint;
//        prevPosition_ = position_;
//        forward_ = { 0,0,1 };
//
//        // デフォルトは全部等速
//        moveTypes_.resize(points_.size(), MoveType::Linear);
//    }
//
//    void RailCameraController::Update(float deltaTime) {
//        if (!moving_ || points_.size() < 2) return;
//
//        // 範囲チェック
//        if (currentSegment_ >= points_.size() - 1) {
//            moving_ = false;
//            return;
//        }
//
//        // 通過許可チェック
//        if (!points_[currentSegment_].passed) return;
//
//        const Vector3& p1 = points_[currentSegment_].controlPoint;
//        const Vector3& p2 = points_[currentSegment_ + 1].controlPoint;
//
//        Vector3 p0 = (currentSegment_ > 0) ? points_[currentSegment_ - 1].controlPoint : p1;
//        Vector3 p3 = (currentSegment_ + 2 < points_.size()) ? points_[currentSegment_ + 2].controlPoint : p2;
//
//        // セグメント長
//        float segmentLength = Length(p2 - p1);
//        if (segmentLength < 0.0001f) segmentLength = 0.0001f;
//
//        // t更新（距離ベース）
//        float deltaT = (speed_ * deltaTime) / segmentLength;
//        t_ += deltaT;
//
//        float tEval = t_;
//
//        //// 🔷 移動タイプ適用
//        //if (moveTypes_[currentSegment_] == MoveType::EaseInOut) {
//        //    // イージング（smoothstep）
//        //    tEval = t_ * t_ * (3.0f - 2.0f * t_);
//        //}
//
//        if (t_ >= 1.0f) {
//            t_ = 0.0f;
//            currentSegment_++;
//            position_ = p2;
//
//            return;
//        }
//
//        // 🔷 Catmull-Rom補間
//        prevPosition_ = position_;
//        position_ = CatmullRom(p0, p1, p2, p3, tEval);
//
//        // 🔷 forward計算
//        Vector3 dir = position_ - prevPosition_;
//        if (Length(dir) > 0.0001f) {
//            forward_ = Normalize(dir);
//        }
//    }
//
//    Vector3 RailCameraController::GetPosition() const {
//        return position_;
//    }
//
//    Vector3 RailCameraController::GetForward() const {
//        return forward_;
//    }
//
//    Vector3 RailCameraController::CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
//        float t2 = t * t;
//        float t3 = t2 * t;
//
//        return 0.5f * (
//            (2.0f * p1) +
//            (-p0 + p2) * t +
//            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
//            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
//            );
//    }
//}