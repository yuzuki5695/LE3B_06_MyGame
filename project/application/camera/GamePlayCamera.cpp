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

        const auto& curve = bezierPoints[0]; 
        railPath_.Build(curve);        // 距離計算
        railSampler_.SetCurve(curve);  // 座標取得用
    }

    void GamePlayCamera::Update(Camera* camera) {
        if (!camera) return;
        Transform* transform = nullptr;
        switch (stateData_.state) {
        case CameraState::Follow:
            UpdateBezier(camera);
            camera->SetTranslate(bezierPos_);
            Vector3 camPos = camera->GetTranslate();
            // プレイヤーの相対移動を取得 
            transform = &target_->GetTransform();
            // 基本位置（カメラ前）
            Vector3 basePos = camPos + Vector3{ 0.0f, -3.0f, 30.0f };
            // Playerからoffset取得
            Vector3 offset = player_->GetMove()->GetRelativePos();
            // 相対移動を加算
            Vector3 finalPos = basePos + offset;
            player_->GetObject3d()->SetTranslate(finalPos);

            break;

        case CameraState::LockOn:
            // 基本位置
            Vector3 cameraPos = camera->GetTranslate() + Vector3{ 0.0f, 0.0f, 10.0f };
            player_->GetObject3d()->SetTranslate(cameraPos);
            break;
        } 
    }

    void GamePlayCamera::UpdateBezier(Camera* camera) {
        const auto& curve = bezierPoints[0];
        if (curve.size() < 2) return;

        // 距離進める
        distance_ += speed;

        // 最大制限
        float maxLen = railPath_.GetTotalLength(); // ← getter追加してね
        if (distance_ >= maxLen) {
            distance_ = maxLen;
        }

        // 距離 → index, t
        railPath_.ConvertDistance(distance_, currentIndex_, t_);

        // 座標取得
        bezierPos_ = railSampler_.Sample(currentIndex_, t_);

        // 接線
        Vector3 tangent = railSampler_.SampleTangent(currentIndex_, t_);
        Vector3 targetForward = Normalize(tangent);

        // 先読み
        float futureT = std::min(t_ + 0.1f, 1.0f);
        Vector3 futurePos = railSampler_.Sample(currentIndex_, futureT);
        Vector3 futureForward = Normalize(futurePos - bezierPos_);

        targetForward = Normalize(Lerp(targetForward, futureForward, 0.3f));

        // 回転補間
        float dot = std::clamp(Dot(forward_, targetForward), -1.0f, 1.0f);
        float angle = acosf(dot);

        float smooth = std::clamp(angle * 0.1f, 0.02f, 0.2f);

        forward_ = Normalize(Slerp(forward_, targetForward, smooth));

        // カメラ適用
        camera->SetLookAt(bezierPos_, bezierPos_ + forward_, { 0,1,0 });
    }
}