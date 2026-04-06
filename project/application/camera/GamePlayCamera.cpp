#include "GamePlayCamera.h"
#include <CameraManager.h>
#include <MathUtil.h>
#include <MatrixVector.h>
#include <Easing.h>
#ifdef min
#undef min
#endif
#include <Player.h>
#ifdef max
#undef max
#endif

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
    }

    void GamePlayCamera::Update(Camera* camera) {
        CameraSet& camSet = CameraManager::GetInstance()->GetCameraSet();
        if (!camera) return;

        if (CameraManager::GetInstance()->GetCameraState().state == CameraDefs::CameraState::Follow) {

            UpdateBezier(camera);
            camera->SetTranslate(bezierPos_);

            Object3d* target = CameraManager::GetInstance()->GetTarget();
            if (target) {
                Vector3 camPos = camera->GetTranslate();
                // プレイヤーの相対移動を取得 
                Transform& transform = target->GetTransform();
                // 基本位置（カメラ前）
                Vector3 basePos = camPos + Vector3{ 0.0f, -3.0f, 30.0f };
                // Playerからoffset取得
                Player* player = CameraManager::GetInstance()->GetPlayer();
                Vector3 offset = player->GetMoveComponent()->GetRelativePos();
                // 相対移動を加算
                Vector3 finalPos = basePos + offset;
                target->SetTranslate(finalPos);
            }
        }
    }

    void GamePlayCamera::UpdateBezier(Camera* camera) {
        if (bezierPoints.empty()) return;

        const auto& curve = bezierPoints[0];

        // 最低4点必要
        if (curve.size() < 4) return;

        // 範囲外防止
        if (currentIndex_ >= curve.size() - 2) {
            return; // end到達
        }

        // Catmull-Rom用の4点
        int i = currentIndex_;

        const Vector3& p0 = curve[std::max(i - 1, 0)].controlPoint;
        const Vector3& p1 = curve[i].controlPoint;
        const Vector3& p2 = curve[i + 1].controlPoint;
        const Vector3& p3 = curve[std::min(i + 2, (int)curve.size() - 1)].controlPoint;

        // 現在位置
        bezierPos_ = CatmullRom(p0, p1, p2, p3, t_);

        // ★ 未来位置（ここ重要）
        float lookAheadT = t_ + 0.05f;

        int nextIndex = i;
        if (lookAheadT >= 1.0f) {
            lookAheadT -= 1.0f;
            nextIndex = std::min(i + 1, (int)curve.size() - 2);
        }

        const Vector3& np0 = curve[std::max(nextIndex - 1, 0)].controlPoint;
        const Vector3& np1 = curve[nextIndex].controlPoint;
        const Vector3& np2 = curve[nextIndex + 1].controlPoint;
        const Vector3& np3 = curve[std::min(nextIndex + 2, (int)curve.size() - 1)].controlPoint;

        Vector3 lookPos = CatmullRom(np0, np1, np2, np3, lookAheadT);

        // ★ 向き設定
        Vector3 up = { 0.0f, 1.0f, 0.0f };
        camera->SetLookAt(bezierPos_, lookPos, up);
        
        // t更新
        t_ += speed * 0.01f;

        // 区間終了
        if (t_ >= 1.0f) {
            t_ = 0.0f;
            currentIndex_++;
            // 到達フラグ（今は全true扱い）
            // curve[currentIndex_].passed = true;
        }
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
}