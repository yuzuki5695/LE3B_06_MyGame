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
    activeSceneCameraType_ = SceneCameraType::Title;
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
    followCam->SetRotate(Vector3{ 0.0f,0.0f,0.0f });
    subCamerasMap_["Default"] = std::move(followCam);

    titlecamera_ =  std::make_unique<TitleCamera>();
    titlecamera_->Initialize();

}

// 更新処理
void CameraManager::Update() {
    Camera* cam = nullptr;
    // --- メインカメラ更新 --- //
    if(mainCamera_) mainCamera_->Update();
    // --- サブカメラ群の更新 --- //
    for(auto& [name, cam] : subCamerasMap_) if(cam) cam->Update();

    titlecamera_->Update(1.0f);

    SetActiveCamera();
}
// アクティブカメラを取得
Camera* CameraManager::GetActiveCamera() {
    switch (Typeview_) {
    case ViewCameraType::Main: return mainCamera_.get();
    case ViewCameraType::Sub:
        if (subCamerasMap_.count("Default")) return subCamerasMap_["Default"].get();
        break;
    }
    return mainCamera_.get();
}

// カメラモード設定
void CameraManager::SetCameraMode(CameraMode mode) {
    currentMode_ = mode;
    SetActiveCamera(); // カメラ共通リソースへ反映
}

// アクティブカメラを共通リソースに設定
void CameraManager::SetActiveCamera() { 
    switch (activeSceneCameraType_) {
    case SceneCameraType::Title:
        activeCamera_ = titlecamera_->GetMainCamera();
        break;

    case SceneCameraType::Gameplay:
        activeCamera_ = mainCamera_.get();
        break;

    case SceneCameraType::GameClear:
        activeCamera_ = mainCamera_.get();
        break;

    case SceneCameraType::GameOver:
        activeCamera_ = mainCamera_.get();
        break;
    }
    // 共通リソースにアクティブカメラを設定
    Object3dCommon::GetInstance()->SetDefaultCamera(activeCamera_);
    ParticleCommon::GetInstance()->SetDefaultCamera(activeCamera_);
}

// ImGui描画
void CameraManager::DrawImGui() {
#ifdef USE_IMGUI
   // ImGui::Begin("Camera Manager");

   // // カメラタイプ切り替え
   // const char* viewTypes[] = { "Main", "Sub" };
   // int currentView = static_cast<int>(Typeview_);
   // if (ImGui::Combo("View Type", &currentView, viewTypes, IM_ARRAYSIZE(viewTypes))) {
   //     SetViewType(static_cast<ViewCameraType>(currentView), switchType_);
   // }

   // // イージング / 瞬間切替
   // const char* switchTypes[] = { "Instant", "Ease" };
   // int currentSwitch = static_cast<int>(switchType_);
   // if (ImGui::Combo("Switch Type", &currentSwitch, switchTypes, IM_ARRAYSIZE(switchTypes))) {
   //     switchType_ = static_cast<CameraSwitchType>(currentSwitch);
   // }

   // // イージング速度
   //// ImGui::DragFloat("Ease Time", &easeTime_, 0.01f, 0.01f, 5.0f);

   // // --- 選択中カメラを取得 ---
   // Camera* cam = GetActiveCamera();
   // if (cam) {
   //     Vector3& pos = cam->GetTranslate();  // 書き換え用参照
   //     Vector3& rot = cam->GetRotate();
   //     // 位置を操作
   //     ImGui::DragFloat3("Position", &pos.x, 0.1f);
   //     // 回転を操作
   //     ImGui::DragFloat3("Rotation", &rot.x, 0.001f);
   // }

   // ImGui::End();
#endif // USE_IMGUI
}
