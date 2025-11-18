#include "ClearCamera.h"
#include <MatrixVector.h>

using namespace MatrixVector;


void ClearCamera::Initialize() {
	// メインカメラ生成、初期化
	maincamera_ = std::make_unique<Camera>();
	maincamera_->SetTranslate(Vector3{ 0.0f,0.0f,0.0f });

    addedInitialOffset_ = false;
}

void ClearCamera::Update() {
    if (target_) {

        Vector3 targetPos = target_->GetWorldPosition();
        Vector3 currentCamPos = maincamera_->GetTranslate();

        switch (followMode_) {

            // --------------------------------
            // ① 初期オフセットを一度だけ保存して追従
            // --------------------------------
        case FollowMode::UseInitialOffset:
            if (!addedInitialOffset_) {

                // --- ここにオンオフ条件を挿入 ---
                if (saveInitialOffsetFlag_) {
                    offset_ = currentCamPos - targetPos;
                    addedInitialOffset_ = true;
                }
            }

            // オフセット方式で追従
            maincamera_->SetTranslate(targetPos + offset_);
            break;


            // --------------------------------
            // ② オフセットを毎フレーム再計算（自由カメラ風）
            // --------------------------------
        case FollowMode::FreeOffset:
            offset_ = currentCamPos - targetPos;
            maincamera_->SetTranslate(targetPos + offset_);
            break;


            // --------------------------------
            // ③ 固定カメラ位置からターゲットだけ注視する
            // --------------------------------
        case FollowMode::FixedLookAt:
            // ★ カメラ位置は「動かさない」
            // maincamera_->SetTranslate(currentCamPos);  // 何もしない

            break;
        }


        if (followMode_ == FollowMode::UseInitialOffset ||
            followMode_ == FollowMode::FreeOffset)
        {
            Vector3 desiredPos = maincamera_->GetTranslate();
            Vector3 dir = targetPos - desiredPos;

            if (Length(dir) > 0.0001f) {
                dir = Normalize(dir);
                float yaw = atan2f(dir.x, dir.z);
                float pitch = -asinf(dir.y);
                maincamera_->SetRotate({ pitch, yaw, 0.0f });
            }
        }
    }

    maincamera_->Update();
}