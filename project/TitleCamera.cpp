#include "TitleCamera.h"
#include <algorithm>

void TitleCamera::Initialize()
{

    maintransform_ = { { 0.0f,0.0f,0.0f },{ 0.0f, 0.0f, 0.0f} };
    std::vector<CameraTransform> subCams = {
     { {0, 10, -50}, {0, 0, 0} },
     { {5, 15, -60}, {0, 30, 0} },
     { {-5, 12, -55}, {0, -30, 0} }
    };
    // サブカメラを追加（例：1つ）
    AddSubCameras(subCams);


}


void TitleCamera::Update()
{

}

void TitleCamera::AddSubCamera(const CameraTransform& trans)
{
    auto cam = std::make_unique<Camera>();
    cam->SetTranslate(trans.translate);
    cam->SetRotate(trans.rotate);
    subCameras_.push_back(std::move(cam));
}

void TitleCamera::AddSubCameras(const std::vector<CameraTransform>& transforms) {
    for (auto& t : transforms) {
        AddSubCamera(t);
    }
}