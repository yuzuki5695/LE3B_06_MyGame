#include "CameraManager.h"
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <MatrixVector.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI

using namespace MatrixVector;

// 静的メンバ変数の定義
std::unique_ptr<CameraManager> CameraManager::instance = nullptr;

// シングルトンインスタンスの取得
CameraManager* CameraManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<CameraManager>();
    }
    return instance.get();
}

// 終了
void CameraManager::Finalize() {
    instance.reset();  // `delete` 不要
}

void CameraManager::Initialize(CameraTransform transform) {
    // 初期状態はデフォルトカメラ
    currentMode_ = CameraMode::Default;
	useFollowCamera_ = false;
    
    // すべてのカメラを作っておく
    defaultCamera_ = new Camera();
    defaultCamera_->SetTranslate(transform.translate);
    defaultCamera_->SetRotate(transform.rotate);
    // オブジェクト追従カメラの生成
    followCamera_ = new Camera();
    // ゲームカメラの生成
	gameCamera_ = new GameCamera();
	gameCamera_->Initialize();
}

void CameraManager::Update() {
    // ★ ゲームカメラは常に更新（表示されなくても）
    if (gameCamera_) { 
        // まだアクティブでない場合は待ち時間を加算
        if (!activated_) {
          //  waitTime_ += 1.0f / 60.0f; // フレームレート60FPS前提
            if (waitTime_ >= 1.0f) {
                gameCamera_->Setmovefige(true); // 3秒後にtrueに
                activated_ = true;
            }
        }

        if (gameCamera_->Getmovefige()) {
            gameCamera_->UpdateObjectPosition();
        }
        gameCamera_->Update(); // ← これはプレイヤー追従のため常に動かす
    }

    switch (currentMode_) {
    case CameraMode::GamePlay:
        if (gameCamera_) {
            if (gameCamera_ && gameCamera_->Getcamera()) {
                gameCamera_->Getcamera()->Update();
            }
        }
        break;
    case CameraMode::Follow:
        if (followCamera_) {
            if (target_) {
                Vector3 targetPos = target_->GetTranslate();
                Vector3 offset(0.0f, 3.0f, -30.0f);
                Vector3 cameraPos = targetPos + offset;

                followCamera_->SetTranslate(cameraPos);

                Vector3 toTarget = targetPos - cameraPos;
                float angleY = std::atan2(toTarget.x, toTarget.z);
                followCamera_->SetRotate(Vector3(0.0f, angleY, 0.0f));
            }
            followCamera_->Update();
        }
        break;
    case CameraMode::Default:
        if (defaultCamera_) {
            // デフォルトカメラの位置と回転を更新
            defaultCamera_->Update();
        }
        break;
    }
}

void CameraManager::SetTarget(Object3d* target) {
    target_ = target;
}

void CameraManager::ToggleCameraMode(bool followMode) {
    useFollowCamera_ = followMode;
}

void CameraManager::DrawImGui() {
#ifdef USE_IMGUI
    ImGui::Begin("CameraManager");

    // ラジオボタンでモード選択
    int mode = static_cast<int>(currentMode_);
    ImGui::Text("Camera Mode");
    if (ImGui::RadioButton("Default", mode == 0)) {
        mode = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Follow", mode == 1)) {
        mode = 1;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("GamePlay", mode == 2)) {
        mode = 2;
    }

    // モードが変わったら切り替える
    CameraMode newMode = static_cast<CameraMode>(mode);
    if (newMode != currentMode_) {
        currentMode_ = newMode;
        SetActiveCamera();
    }

    // 現在のカメラを取得して編集
    Camera* activeCamera = nullptr;
    const char* modeName = "";
    const char* label = "Translate";
    const char* label2 = "Rotate";

    switch (currentMode_) {
    case CameraMode::Default:
        activeCamera = defaultCamera_;
        modeName = "Default";
        break;
    case CameraMode::Follow:
        activeCamera = followCamera_;
        modeName = "Follow";
        break;
    case CameraMode::GamePlay:
        activeCamera = gameCamera_->Getcamera();
        modeName = "GamePlay";
        break;
    }
    ImGui::Text("Current Mode: %s", modeName);

    if (activeCamera) {
        Vector3 pos = activeCamera->GetTranslate();
        Vector3 rotate = activeCamera->GetRotate();
        if (ImGui::DragFloat3(label, &pos.x, 0.01f)) {
            activeCamera->SetTranslate(pos);
        }
        if (ImGui::DragFloat3(label2, &rotate.x, 0.01f)) {
            activeCamera->SetRotate(rotate);
        }
    }

    bool moveFlag = gameCamera_->Getmovefige();
    if (ImGui::Checkbox("isBezier", &moveFlag)) {
        gameCamera_->Setmovefige(moveFlag);
    }
    Vector3 bezierPos = gameCamera_->GetbezierPos();
    Vector3 rotate = gameCamera_->Getcamera()->GetRotate();
    if (ImGui::DragFloat3("BezierTranslate", &bezierPos.x, 0.01f)) {
        gameCamera_->SetbezierPos(bezierPos);
    }
    //if (ImGui::DragFloat3(label2, &rotate.x, 0.01f)) {
    //    gameCamera_->Getcamera()->SetRotate(rotate);
    //}

    ImGui::End();
#endif // USE_IMGUI
}

Camera* CameraManager::GetActiveCamera() {
    switch (currentMode_) {
    case CameraMode::Follow:
        return followCamera_ ? followCamera_ : defaultCamera_;
    case CameraMode::GamePlay:
        return gameCamera_ ? gameCamera_->Getcamera() : nullptr;
    case CameraMode::Default:
    default:
        return defaultCamera_;
    }
}

void CameraManager::SetCameraMode(CameraMode mode) {
    currentMode_ = mode;
    SetActiveCamera(); // カメラ共通リソースへ反映
}

void CameraManager::SetActiveCamera() {
    Camera* active = GetActiveCamera();
    if (active) {
        Object3dCommon::GetInstance()->SetDefaultCamera(active);
        ParticleCommon::GetInstance()->SetDefaultCamera(active);
    }
}