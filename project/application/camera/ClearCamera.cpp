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

        // 一度だけカメラの初期オフセットを記録
        if (!addedInitialOffset_) {
            offset_ = maincamera_->GetTranslate() - targetPos;
            //addedInitialOffset_ = true;
        }

        // カメラをターゲットの周囲に維持
        Vector3 desiredPos = targetPos + offset_;
        maincamera_->SetTranslate(desiredPos);

        // ---- ターゲット方向を向き続ける ----
        Vector3 dir = targetPos - desiredPos;
        if (Length(dir) > 0.0001f) {
            dir = Normalize(dir);
            float yaw = atan2f(dir.x, dir.z);
            float pitch = -asinf(dir.y);
            maincamera_->SetRotate({ pitch, yaw, 0.0f });
        }
    }

    maincamera_->Update();
}