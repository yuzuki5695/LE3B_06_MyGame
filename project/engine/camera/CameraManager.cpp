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
// 初期化
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
    moveFlag = false; 
    gameCamera_->Setmovefige(moveFlag);
}
// 更新処理
void CameraManager::Update() {
    switch (currentMode_) {
    case CameraMode::GamePlay: 
        if (gameCamera_) {
			gameCamera_->Update();
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
// 追従対象をセット（nullptrなら追従なし）
void CameraManager::SetTarget(Object3d* target) {
    target_ = target;
}
// カメラモード切替
void CameraManager::ToggleCameraMode(bool followMode) {
    useFollowCamera_ = followMode;
}
// ImGui描画
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
    ImGui::SameLine();  
    if (ImGui::RadioButton("Event", mode == 3)) { mode = 3; } 

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
        if (currentMode_ == CameraMode::GamePlay) {
            if (ImGui::Checkbox("isBezier", &moveFlag)) {
                gameCamera_->Setmovefige(moveFlag);
            }
        }

    }
    ImGui::End();
#endif // USE_IMGUI
}
// アクティブカメラを取得
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
// カメラモード設定
void CameraManager::SetCameraMode(CameraMode mode) {
    currentMode_ = mode;
    SetActiveCamera(); // カメラ共通リソースへ反映
}
// アクティブカメラを共通リソースに設定
void CameraManager::SetActiveCamera() {
    Camera* active = GetActiveCamera();
    if (active) {
        Object3dCommon::GetInstance()->SetDefaultCamera(active);
        ParticleCommon::GetInstance()->SetDefaultCamera(active);
    }
}