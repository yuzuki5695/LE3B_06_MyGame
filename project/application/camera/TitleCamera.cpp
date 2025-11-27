#include "TitleCamera.h"

void TitleCamera::Initialize() {
    // カメラの値の初期化
    maintransform_ = { { 0.0f,0.0f,0.0f },{ 0.0f, 0.0f, 0.0f} };
    std::vector<CameraTransform> subCams = {
     { {0, 10, -50}, {0, 0, 0} },
     { {5, 15, -60}, {0, 30, 0} },
    };
    // サブカメラを追加
    AddSubCameras(subCams);
}

void TitleCamera::Update(){}

void TitleCamera::AddSubCamera(const CameraTransform& trans) {
    // 新しいサブカメラインスタンスを生成
    std::shared_ptr<Camera> cam = std::make_unique<Camera>();

    // 渡されたCameraTransformを元に、位置と角度を設定
    cam->SetTranslate(trans.translate);
    cam->SetRotate(trans.rotate);

    // サブカメラリストへ追加（unique_ptrの所有権を移動
    subcameras_.push_back(cam);
}

void TitleCamera::AddSubCameras(const std::vector<CameraTransform>& transforms) {
    for (const CameraTransform& trans : transforms) {
        // サブカメラを複数登録する
        AddSubCamera(trans);
    }
}