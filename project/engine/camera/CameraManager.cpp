#include "CameraManager.h"
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <MatrixVector.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
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
    // 初期化はメインカメラでモードはデフォルト
    Typeview_ = ViewCameraType::Main; 
    currentMode_ = CameraMode::Default;
    switchType_ = CameraSwitchType::Instant;
    
    // --- メインカメラの初期化--- //
    mainCamera_ = std::make_unique<Camera>();
    mainCamera_->SetTranslate(transform.translate);
    mainCamera_->SetRotate(transform.rotate);

    // --- サブカメラ群登録 --- //
    subCamerasMap_.clear();
    // 固定カメラ
    std::unique_ptr<Camera> followCam = std::make_unique<Camera>();
    followCam->SetTranslate(Vector3{ 0.0f,-10.0f,-50.0f });
    followCam->SetRotate(Vector3{0.0f,0.0f,0.0f});
    subCamerasMap_["Default"] = std::move(followCam);
        
    deltaTime_ = 3.0f;

}

// 更新処理
void CameraManager::Update() {
    Camera* cam = nullptr;
    // --- メインカメラ更新 --- //
    if(mainCamera_) mainCamera_->Update();
    // --- サブカメラ群の更新 --- //
    for(auto& [name, cam] : subCamerasMap_) if(cam) cam->Update();
    
    tempCamera_.Update();

    // 補間中
    if(easing_ && targetCamera_) {
        easeTimer_ += 0.0167f; // ← deltaTime_ ではなく実際のフレーム経過時間を使う
        float t = std::clamp(easeTimer_ / easeTime_, 0.0f, 1.0f);

        // 線形補間（必要ならスムーズステップ）
        float easeT = t * t * (3 - 2 * t); // SmoothStep

        tempCamera_.SetTranslate(cameraLerp(startPos_, targetPos_, easeT));
        tempCamera_.SetRotate(cameraLerp(startRot_, targetRot_, easeT));

        if(t >= 1.0f) {
            // 補間完了
            easing_ = false;
            targetCamera_ = nullptr;

            // 補間後はアクティブカメラを目標に合わせる
            Typeview_ = (Typeview_ == ViewCameraType::Main) ? ViewCameraType::Sub : ViewCameraType::Main;
        }
    }

    SetActiveCamera();
}

// アクティブカメラを取得
Camera* CameraManager::GetActiveCamera() {
    if(easing_ && targetCamera_) return &tempCamera_;
    switch(Typeview_) {
        case ViewCameraType::Main: return mainCamera_.get();
        case ViewCameraType::Sub:  
            if(subCamerasMap_.count("Default")) return subCamerasMap_["Default"].get();
            break;
    }
    return mainCamera_.get();
}

void CameraManager::SetViewType(ViewCameraType type, CameraSwitchType switchType)
{
    Camera* targetCam = nullptr;
    switch(type) {
        case ViewCameraType::Main: targetCam = mainCamera_.get(); break;
        case ViewCameraType::Sub:
            if(subCamerasMap_.count("Default")) targetCam = subCamerasMap_["Default"].get();
            break;
    }
    if(!targetCam) return;

    switchType_ = switchType;

    if(switchType == CameraSwitchType::Ease) {
        targetCamera_ = targetCam;

        Camera* currentCam = GetActiveCamera();
        startPos_ = currentCam->GetTranslate();
        startRot_ = currentCam->GetRotate();

        targetPos_ = targetCamera_->GetTranslate();
        targetRot_ = targetCamera_->GetRotate();

        easeTimer_ = 0.0f;
        easeTime_ = 3.0f;  // ここで3秒間に固定
        easing_ = true;
    } else {
        Typeview_ = type;
        easing_ = false;
        targetCamera_ = nullptr;
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
    // 共通リソースにアクティブカメラを設定
    Object3dCommon::GetInstance()->SetDefaultCamera(active);
    ParticleCommon::GetInstance()->SetDefaultCamera(active);
}

// ImGui描画
void CameraManager::DrawImGui() {
#ifdef USE_IMGUI
    ImGui::Begin("Camera Manager");

    // カメラタイプ切り替え
    const char* viewTypes[] = { "Main", "Sub" };
    int currentView = static_cast<int>(Typeview_);
    if (ImGui::Combo("View Type", &currentView, viewTypes, IM_ARRAYSIZE(viewTypes))) {
        SetViewType(static_cast<ViewCameraType>(currentView), switchType_);
    }

    // イージング / 瞬間切替
    const char* switchTypes[] = { "Instant", "Ease" };
    int currentSwitch = static_cast<int>(switchType_);
    if (ImGui::Combo("Switch Type", &currentSwitch, switchTypes, IM_ARRAYSIZE(switchTypes))) {
        switchType_ = static_cast<CameraSwitchType>(currentSwitch);
    }

    // イージング速度
    ImGui::DragFloat("Ease Time", &easeTime_, 0.01f, 0.01f, 5.0f);

    // --- 選択中カメラを取得 ---
    Camera* cam = GetActiveCamera();
    if (cam) {
        Vector3& pos = cam->GetTranslate();  // 書き換え用参照
        Vector3& rot = cam->GetRotate();
        // 位置を操作
        ImGui::DragFloat3("Position", &pos.x, 0.1f);
        // 回転を操作
        ImGui::DragFloat3("Rotation", &rot.x, 0.001f);
    }

    ImGui::End();
#endif // USE_IMGUI
}
