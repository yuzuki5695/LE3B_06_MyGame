#include "GameClearCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>

using namespace MatrixVector;

void GameClearCamera::Initialize() {
    // カメラの値の初期化
    transform_ = { { 0.0f,0.0f,0.0f },{ 0.0f, 0.0f, 0.0f} };


}

void GameClearCamera::Update() {
    // カメラ位置は固定（maincamera_->SetTranslate を呼ばない）

    Vector3 camPos = transform_.translate;
    Vector3 targetPos = target_->GetWorldPosition();
    Vector3 dir = targetPos - camPos;

    // 正規化（ゼロ割対策）
    if (Length(dir) > 0.0001f) {
        dir = Normalize(dir);

        // --- ターゲット方向へ回転を設定 ---
        float yaw = atan2f(dir.x, dir.z);
        float pitch = -asinf(dir.y);

        transform_.rotate = { pitch, yaw, 0.0f };
    }
}

void GameClearCamera::AddSubCamera(const CameraTransform& trans) {
    // 新しいサブカメラインスタンスを生成
    std::unique_ptr<Camera> cam = std::make_unique<Camera>();

    // 渡されたCameraTransformを元に、位置と角度を設定
    cam->SetTranslate(trans.translate);
    cam->SetRotate(trans.rotate);

    // サブカメラリストへ追加（unique_ptrの所有権を移動
    subcameras_.push_back(std::move(cam));
}

void GameClearCamera::AddSubCameras(const std::vector<CameraTransform>& transforms) {
    for (const CameraTransform& trans : transforms) {
        // サブカメラを複数登録する
        AddSubCamera(trans);
    }
}