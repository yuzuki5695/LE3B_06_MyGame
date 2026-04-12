#include "GameOverCamera.h"
#include <CameraManager.h>

using namespace MyEngine;

namespace MyGame {

    void GameOverCamera::Initialize(MyEngine::Camera* camera) {
        if (!camera) return;

        //camera->SetRotate({ 0.0f, 0.0f, 0.0f });
        //camera->SetTranslate({ 0.0f, 0.0f, -50.0f });

        //auto camMgr = CameraManager::GetInstance();

        //// メインカメラの位置取得
        //MyEngine::Vector3 mainPos = camera->GetTranslate();

        //// サブカメラ生成
        //std::unique_ptr<MyEngine::Camera> subCam = std::make_unique<MyEngine::Camera>();

        //// Zを -50 離す
        //MyEngine::Vector3 subPos = mainPos;
        //subPos.z -= 50.0f;

        //subCam->SetTranslate(subPos);
        //subCam->SetRotate(camera->GetRotate());

        //// 登録
        //camMgr->GetCameraSet().AddSubCamera("Sub1", std::move(subCam));

        //// 有効化
        //camMgr->GetCameraSet().SetActiveSubCamera("Sub1");
    }
    
    void GameOverCamera::Update(MyEngine::Camera* camera) {
        CameraSet& camSet = CameraManager::GetInstance()->GetCameraSet();
        if (!camera) return;

        // サブカメラ中なら何もしない
        if (camSet.IsUsingSubCamera()) {
            return;
        }
    
    
    
    }
}