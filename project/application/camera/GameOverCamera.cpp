#include "GameOverCamera.h"

void GameOverCamera::Initialize() {
    // カメラの値の初期化
    transform_ = { { 0.0f,0.0f,0.0f },{ 0.0f, 0.0f, 0.0f} };
}

void GameOverCamera::Update(){}

void GameOverCamera::AddSubCamera(const CameraTransform& trans) {
    // 新しいサブカメラインスタンスを生成
    std::unique_ptr<Camera> cam = std::make_unique<Camera>();

    // 渡されたCameraTransformを元に、位置と角度を設定
    cam->SetTranslate(trans.translate);
    cam->SetRotate(trans.rotate);

    // サブカメラリストへ追加（unique_ptrの所有権を移動
    subcameras_.push_back(std::move(cam));
}

void GameOverCamera::AddSubCameras(const std::vector<CameraTransform>& transforms) {
    for (const CameraTransform& trans : transforms) {
        // サブカメラを複数登録する
        AddSubCamera(trans);
    }
}