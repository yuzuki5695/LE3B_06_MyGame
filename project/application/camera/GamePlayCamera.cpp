#include "GamePlayCamera.h"
#include <CameraManager.h>
#include <MathUtil.h>
#include <MatrixVector.h>
#include <Easing.h>

using namespace MyEngine;
using namespace MathUtil;
using namespace MatrixVector;
using namespace Easing;

namespace MyGame {

    void GamePlayCamera::Initialize(Camera* camera) {
        CameraDefs::StateData data;
        data.type = CameraDefs::CameraType::Main;
        //data.state = CameraDefs::CameraState::Default;
        data.state = CameraDefs::CameraState::Follow;

        Jsondata_ = std::make_unique<CurveJsonLoader>();
        bezierPoints = Jsondata_->LoadBezierFromJSON("Resources/levels/bezier.json");

        // カメラマネージャに状態をセット
        CameraManager::GetInstance()->SetCameraState(data);

        //  deltaTime_ = 1.0f / 60.0f;

    }

    void GamePlayCamera::Update(Camera* camera) {
        CameraSet& camSet = CameraManager::GetInstance()->GetCameraSet();
        if (!camera) return;

        if (CameraManager::GetInstance()->GetCameraState().state == CameraDefs::CameraState::Follow) {

            UpdateBezier(camera);
            // 🔥 追加：回転更新
            //UpdateCameraRotation(camera);
            camera->SetTranslate(bezierPos_);

            auto target = CameraManager::GetInstance()->GetTarget();
            if (target) {
                Vector3 camPos = camera->GetTranslate();
                Vector3 forward = camera->GetForward();
                // カメラの前に10ユニット置く
                float distance = 30.0f;
                Vector3 objectPos = camPos + forward * distance;
                // 反映
                target->SetTranslate(objectPos);
            }
        }
    }

    void GamePlayCamera::UpdateBezier(Camera* camera) {
        // データチェック
        if (bezierPoints.empty()) return;

        const std::vector<BezierPoint>& curve = bezierPoints[currentCurve_];

        // 最低2点必要
        if (curve.size() < 2) return;

        // 範囲チェック（最後まで行ったら止める）
        if (currentSegment_ >= static_cast<int>(curve.size()) - 1) {
            return;
        }

        // 今の2点
        const MyEngine::Vector3& p0 = curve[currentSegment_].controlPoint;
        const MyEngine::Vector3& p1 = curve[currentSegment_ + 1].controlPoint;

        // 補間
        bezierPos_ = Lerp(p0, p1, t_);

        // カメラに適用
        camera->SetTranslate(bezierPos_);

        // 進行
        t_ += speed_;

        // 次の区間へ
        if (t_ >= 1.0f) {
            t_ = 0.0f;
            currentSegment_++;
        }
    }

    void GamePlayCamera::UpdateCameraRotation(Camera* camera) {

        if (bezierPoints.empty()) return;

        const std::vector<BezierPoint>& curve = bezierPoints[currentCurve_];
        if (curve.size() < 2) return;

        Vector3 targetForward;

        if (currentSegment_ < static_cast<int>(curve.size()) - 1) {

            Vector3 next = curve[currentSegment_ + 1].controlPoint;

            Vector3 next2 = (currentSegment_ + 2 < static_cast<int>(curve.size()))
                ? curve[currentSegment_ + 2].controlPoint
                : next;

            targetForward = Normalize(
                (next - bezierPos_) * 0.7f +
                (next2 - next) * 0.3f
            );
        } else {
            targetForward = prevForward;
        }

        float dot = std::clamp(Dot(prevForward, targetForward), -1.0f, 1.0f);
        float angle = acosf(dot);

        float smooth = std::clamp(angle * 0.1f, 0.02f, 0.15f);

        Vector3 newForward = Slerp(prevForward, targetForward, smooth);
        newForward = Normalize(newForward);

        prevForward = newForward;
        forward_ = newForward;

        // 🔥 ここが重要
        Vector3 eye = bezierPos_;
        Vector3 target = eye + forward_;
        Vector3 up = { 0,1,0 };

        camera->SetLookAt(eye, target, up);
    }

    //void GamePlayCamera::UpdateBezierMovement() {
    //    // 現在のセグメント start / end
    //    const Vector3& start = bezierPoints[currentSegment].controlPoint;
    //    const Vector3& end = bezierPoints[currentSegment + 1].controlPoint;
    //    Vector3 oldPos = bezierPos_;

    //    // --- 直線モード（start → point_01） ---
    //    if (currentSegment == 0) {
    //        Vector3 dir = end - bezierPos_;
    //        float dist = Length(dir);


    //        if (dist <= speed) {
    //            bezierPos_ = end;
    //            bezierPoints[currentSegment].passed = true;
    //            currentSegment++;
    //        } else {
    //            bezierPos_ += Normalize(dir) * speed; // ベクトル直進
    //            currentRailLength_ += Length(Normalize(dir) * speed); // ★追加
    //        }
    //        return;
    //    }

    //    // --- 補完モード（それ以降） ---
    //    // 現在のセグメント長を取得（Catmull-Rom補間を距離基準で進めるため）
    //    Vector3 p0 = (currentSegment > 0) ? bezierPoints[currentSegment - 1].controlPoint : start;
    //    Vector3 p1 = start;
    //    Vector3 p2 = end;
    //    Vector3 p3 = (currentSegment + 2 < bezierPoints.size()) ? bezierPoints[currentSegment + 2].controlPoint : end;

    //    // 簡易的にセグメント長を「直線距離」で近似
    //    float segmentLength = Length(p2 - p1);
    //    if (segmentLength < 0.0001f) segmentLength = 0.0001f;

    //    // t の増分を距離基準で計算
    //    float deltaT = speed / segmentLength;
    //    t_ += deltaT;

    //    if (t_ >= 1.0f) {
    //        t_ = 0.0f;
    //        currentSegment++;
    //        bezierPos_ = end;

    //        if (currentSegment >= bezierPoints.size() - 1) {
    //            movefige = false;
    //            return;
    //        }
    //    } else {
    //        // Catmull-Rom 補間
    //        bezierPos_ = CatmullRom(p0, p1, p2, p3, t_);
    //    }
    //    // 移動距離を累積
    //    currentRailLength_ += Length(bezierPos_ - oldPos);
    //    prevPos_ = bezierPos_;
    //}

    //Vector3 GamePlayCamera::CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
    //    float t2 = t * t;
    //    float t3 = t2 * t;

    //    return 0.5f * (
    //        (2.0f * p1) +
    //        (-p0 + p2) * t +
    //        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
    //        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    //        );
    //}
}