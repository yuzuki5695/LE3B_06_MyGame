#include "GamePlayCamera.h"
#include <CameraManager.h>
#include <MathUtil.h>
#include <MatrixVector.h>
#include <Easing.h>
#include <Player.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace MyEngine;
using namespace CameraDefs;
using namespace MathUtil;
using namespace MatrixVector;
using namespace Easing;

namespace MyGame {

    void GamePlayCamera::Initialize(Camera* camera) {
		// カメラのステートを設定
        stateData_.type = CameraType::Main;
        stateData_.state = CameraState::Follow;

        Jsondata_ = std::make_unique<CurveJsonLoader>();
        bezierPoints = Jsondata_->LoadBezierFromJSON("Resources/levels/bezier.json");

        BuildRailInfo(bezierPoints[0]);
    }

    void GamePlayCamera::Update(Camera* camera) {
        CameraState camState = CameraManager::GetInstance()->GetCameraState().state;
        if (!camera) return;
     
        //Vector3 Pos = camera->GetTranslate() + Vector3{ 0.0f, 0.0f, 30.0f };
        //player_->GetObject3d()->SetTranslate(Pos);


        if (camState == CameraState::Follow) {
            UpdateBezier(camera);
            camera->SetTranslate(bezierPos_);

            if (target_) {
                Vector3 camPos = camera->GetTranslate();
                // プレイヤーの相対移動を取得 
                Transform& transform = target_->GetTransform();
                // 基本位置（カメラ前）
                Vector3 basePos = camPos + Vector3{ 0.0f, -3.0f, 30.0f };
                // Playerからoffset取得
                Vector3 offset = player_->GetMove()->GetRelativePos();
                // 相対移動を加算
                Vector3 finalPos = basePos + offset;
                player_->GetObject3d()->SetTranslate(finalPos);
            }
        }
    }

    void GamePlayCamera::UpdateBezier(Camera* camera) {
        if (bezierPoints.empty()) return;
        const auto& curve = bezierPoints[0];
        if (curve.size() < 2) return;

        // ① 距離を進める
        distance_ += speed;

        // ② 終端処理
        if (distance_ >= railInfo_.totalLength) {
            distance_ = railInfo_.totalLength;
        }
        // ③ 距離 → index + t に変換
        ConvertDistanceToSegment(distance_);

        // ④ 座標取得
        bezierPos_ = GetCatmullRomPoint(curve, currentIndex_, t_);

        // 制御点取得
        auto GetPoint = [&](int i) -> const Vector3& {
            i = std::clamp(i, 0, (int)curve.size() - 1);
            return curve[i].controlPoint;
            };

        const Vector3& p0 = GetPoint(currentIndex_ - 1);
        const Vector3& p1 = GetPoint(currentIndex_);
        const Vector3& p2 = GetPoint(currentIndex_ + 1);
        const Vector3& p3 = GetPoint(currentIndex_ + 2);

        // 接線取得
        Vector3 tangent = CatmullRomTangent(p0, p1, p2, p3, t_);
        Vector3 targetForward = Normalize(tangent);

        float futureT = t_ + 0.1f;
        futureT = std::min(futureT, 1.0f);

        Vector3 futurePos = GetCatmullRomPoint(curve, currentIndex_, futureT);
        Vector3 futureForward = Normalize(futurePos - bezierPos_);

        targetForward = Normalize(Lerp(targetForward, futureForward, 0.3f));
     
        // 角度計算
        float dot = std::clamp(Dot(forward_, targetForward), -1.0f, 1.0f);
        float angle = acosf(dot);

        // 角度ベース補間係数
        float smooth = std::clamp(angle * 0.1f, 0.02f, 0.2f);

        // Slerp補間
        forward_ = Normalize(Slerp(forward_, targetForward, smooth));

        camera->SetLookAt(bezierPos_, bezierPos_ + forward_, { 0.0f, 1.0f, 0.0f });
    }

    Vector3 GamePlayCamera::CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
        float t2 = t * t;
        float t3 = t2 * t;

        return 0.5f * (
            (2.0f * p1) +
            (-p0 + p2) * t +
            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
            );
    }

    void GamePlayCamera::BuildRailInfo(const std::vector<BezierPoint>& curve) {
        railInfo_.segmentLengths.clear();
        railInfo_.totalLength = 0.0f;

        if (curve.size() < 2) return;

        for (int i = 0; i < (int)curve.size() - 1; i++) {
            float length = CalculateSegmentLength(curve, i);

            railInfo_.segmentLengths.push_back(length);
            railInfo_.totalLength += length;
        }
    }

    float GamePlayCamera::CalculateSegmentLength(const std::vector<BezierPoint>& curve, int index) {
        const int div = 10;

        float length = 0.0f;

        Vector3 prev = GetCatmullRomPoint(curve, index, 0.0f);

        for (int j = 1; j <= div; j++) {
            float t = j / (float)div;

            Vector3 pos = GetCatmullRomPoint(curve, index, t);

            length += Length(pos - prev);
            prev = pos;
        }

        return length;
    }
    
    Vector3 GamePlayCamera::CatmullRomTangent(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
        float t2 = t * t;

        return 0.5f * (
            (-p0 + p2) +
            2.0f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t +
            3.0f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t2
            );
    }

    Vector3 GamePlayCamera::GetCatmullRomPoint(const std::vector<BezierPoint>& curve, int index, float t) {
        auto GetPoint = [&](int i) -> const Vector3& {
            i = std::clamp(i, 0, (int)curve.size() - 1);
            return curve[i].controlPoint;
            };

        const Vector3& p0 = GetPoint(index - 1);
        const Vector3& p1 = GetPoint(index);
        const Vector3& p2 = GetPoint(index + 1);
        const Vector3& p3 = GetPoint(index + 2);

        return CatmullRom(p0, p1, p2, p3, t);
    }
    void GamePlayCamera::ConvertDistanceToSegment(float distance) {
        float remain = distance;

        int segment = 0;

        // どのセグメントか探す
        while (segment < (int)railInfo_.segmentLengths.size()) {
            if (remain <= railInfo_.segmentLengths[segment]) break;

            remain -= railInfo_.segmentLengths[segment];
            segment++;
        }

        // 安全化
        segment = std::clamp(segment, 0, (int)railInfo_.segmentLengths.size() - 1);

        currentIndex_ = segment;

        // tを算出
        float segmentLength = railInfo_.segmentLengths[segment];

        if (segmentLength > 0.0f) {
            t_ = remain / segmentLength;
        } else {
            t_ = 0.0f;
        }
    }
    void GamePlayCamera::ConvertDistanceToSegmentInternal(float distance, int& outIndex, float& outT) {
        float remain = distance;
        int segment = 0;

        while (segment < (int)railInfo_.segmentLengths.size()) {
            if (remain <= railInfo_.segmentLengths[segment]) break;
            remain -= railInfo_.segmentLengths[segment];
            segment++;
        }

        segment = std::clamp(segment, 0, (int)railInfo_.segmentLengths.size() - 1);

        float segmentLength = railInfo_.segmentLengths[segment];

        outIndex = segment;
        outT = (segmentLength > 0.0f) ? (remain / segmentLength) : 0.0f;
    }
}