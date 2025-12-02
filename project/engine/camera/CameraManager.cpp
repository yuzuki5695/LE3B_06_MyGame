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
    // 各構造体の初期化
    Typeview_ = ViewCameraType::Main;                     // メイン
    activeSceneCameraType_ = SceneCameraType::Title;      // タイトル(シーンマネージャで管理されているため変える必要なし)
    currentMode_ = CameraMode::Default;                   // デフォルト
    switchType_ = CameraSwitchType::Instant;              // 瞬間的に切り替わる
    maintrans_ = transform;                               // 引数を入れる
    // --- メインカメラの生成、初期化--- //
    mainCamera_ = std::make_unique<Camera>();
    mainCamera_->SetTranslate(maintrans_.translate);
    mainCamera_->SetRotate(maintrans_.rotate);
    activeCamera_ = mainCamera_.get();  // 初期化時はメインカメラをアクティブにする

    // タイトル用カメラの生成、初期化
    titlecamera_ = std::make_unique<TitleCamera>();
    titlecamera_->Initialize();
    // ゲームオーバー用カメラの生成、初期化
    gameovercamera_ = std::make_unique<GameOverCamera>();
    gameovercamera_->Initialize();

    // 初期のシーンはタイトルカメラ
    currentSceneCamera_ = titlecamera_.get();
    sceneCameraJustChanged_ = true;
}

// 更新処理
void CameraManager::Update() {
    // シーンカメラが変わったら一度だけTransformとサブカメラを反映
    if (sceneCameraJustChanged_) {
        // メインカメラの Transform を更新
        maintrans_ = currentSceneCamera_->GetMainTransform();
        // 前のサブカメラを破棄
        subCamerasMap_.clear();
        // 新しいシーンに切り替えるときのみムーブ
        RegisterSubCameras(std::move(currentSceneCamera_->MoveSubCameras()), "SubCamera");
        sceneCameraJustChanged_ = false;
    }

    // 現在のシーンカメラの更新
    currentSceneCamera_->Update();

    // メインカメラに値を反映
    mainCamera_->SetTranslate(maintrans_.translate);
    mainCamera_->SetRotate(maintrans_.rotate);
    mainCamera_->Update();

    // サブカメラ更新
    for (std::pair<const std::string, std::unique_ptr<Camera>>& subcameras : subCamerasMap_) {
        subcameras.second->Update();
    }
    // アクティブ登録
    SetActiveCamera();
}

// ViewCameraType → const char*
const char* ToString(CameraTypes::ViewCameraType type) {
    switch (type) {
    case CameraTypes::ViewCameraType::Main: return "Main";
    case CameraTypes::ViewCameraType::Sub: return "Sub";
    }
    return "Unknown";
}

// SceneCameraType → const char*
const char* ToString(CameraTypes::SceneCameraType type) {
    switch (type) {
    case CameraTypes::SceneCameraType::Title: return "Title";
    case CameraTypes::SceneCameraType::Gameplay: return "Gameplay";
    case CameraTypes::SceneCameraType::GameClear: return "GameClear";
    case CameraTypes::SceneCameraType::GameOver: return "GameOver";
    }
    return "Unknown";
}

// CameraMode → const char*
const char* ToString(CameraTypes::CameraMode mode) {
    switch (mode) {
    case CameraTypes::CameraMode::Default: return "Default";
    case CameraTypes::CameraMode::Follow: return "Follow";
    case CameraTypes::CameraMode::Event: return "Event";
    case CameraTypes::CameraMode::Transition: return "Transition";
    case CameraTypes::CameraMode::Debug: return "Debug";
    }
    return "Unknown";
}

// ImGui描画
void CameraManager::DrawImGui() {
#ifdef USE_IMGUI
    ImGui::Begin("Camera Manager");

    // ================================================================
    // 状態を表示（変更不可）
    // ================================================================
    ImGui::Text("=== Current Camera Status ===");

    ImGui::Text("Scene Camera Type : %s", ToString(activeSceneCameraType_));
    ImGui::Text("View Camera Type : %s", ToString(Typeview_));
    ImGui::Text("Camera Mode : %s", ToString(currentMode_));

    // 現在のアクティブカメラを文字列で表示
    Camera* activeCam = GetActiveCamera();
    if (activeCam) {
        std::string activeCamName = "Main Camera";

        // サブカメラに含まれているか確認
        for (auto& [name, cam] : subCamerasMap_) {
            if (cam.get() == activeCam) {
                activeCamName = name; // 見つかったら名前をセット
                break;
            }
        }

        ImGui::Text("Active Camera : %s", activeCamName.c_str());
    }

    // 今のシーンにあるメインとサブカメラの個数を表示
    ImGui::Text("Main Camera Count: %d", mainCamera_ ? 1 : 0);
    ImGui::Text("Sub Camera Count: %d", static_cast<int>(subCamerasMap_.size()));

    ImGui::Separator();

    // ================================================================
    // ① アクティブカメラ種別（Main / Sub）
    // ================================================================

    // アクティブカメラ切替（ラジオボタン形式）
    ImGui::Text("Active Camera");

    // ラジオボタンのIDを動的に生成
    int radioIdx = 0;

    // メインカメラ
    if (ImGui::RadioButton(("Main Camera##" + std::to_string(radioIdx++)).c_str(),
        Typeview_ == ViewCameraType::Main && activeSubCameraName_.empty()))
    {
        Typeview_ = ViewCameraType::Main;
        activeSubCameraName_.clear();
        SetActiveCamera();
    }
    // サブカメラ群
    if (!subCamerasMap_.empty()) {
        for (auto& [name, cam] : subCamerasMap_) {
            if (ImGui::RadioButton((name + "##" + std::to_string(radioIdx++)).c_str(),
                Typeview_ == ViewCameraType::Sub && activeSubCameraName_ == name))
            {
                Typeview_ = ViewCameraType::Sub;
                activeSubCameraName_ = name;
                SetActiveCamera();
            }
        }
    }

    ImGui::Separator();
    // ================================================================
    // ② 個々のカメラ Debug（transform 調整 UI）
    // ================================================================

    if (ImGui::TreeNode("Main Camera")) {
        Vector3 pos = maintrans_.translate;
        Vector3 rot = maintrans_.rotate;

        if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
            maintrans_.translate = pos;
        }
        if (ImGui::DragFloat3("Rotation", &rot.x, 0.1f)) {
            maintrans_.rotate = rot;
        }
        ImGui::TreePop();
    }


    if (!subCamerasMap_.empty() && ImGui::TreeNode("Sub Cameras")) {
        int idx = 0;
        for (auto& [name, cam] : subCamerasMap_) {
            std::string label = name + "##" + std::to_string(idx++);
            if (ImGui::TreeNode(label.c_str())) {
                cam->DebugUpdate();
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
#endif
}

// アクティブカメラを取得
Camera* CameraManager::GetActiveCamera() {
    switch (Typeview_) {
    case ViewCameraType::Main:
        return mainCamera_.get();
    case ViewCameraType::Sub:
        if (!activeSubCameraName_.empty() && subCamerasMap_.count(activeSubCameraName_)) {
            return subCamerasMap_[activeSubCameraName_].get();
        }
        else if (!subCamerasMap_.empty()) {
            return subCamerasMap_.begin()->second.get(); // 最初のサブカメラ
        }
        break;
    }
    return mainCamera_.get();
}

// カメラモード設定
void CameraManager::SetCameraMode(CameraMode mode) {
    currentMode_ = mode;   // モードを変更
    SetActiveCamera();     // カメラ共通リソースへ反映
}

// アクティブカメラを共通リソースに設定
void CameraManager::SetActiveCamera() { 
    activeCamera_ = GetActiveCamera(); // アクティブカメラを取得
    // 共通リソースにアクティブカメラを設定
    Object3dCommon::GetInstance()->SetDefaultCamera(activeCamera_);
    ParticleCommon::GetInstance()->SetDefaultCamera(activeCamera_);
}

// サブカメラを一括登録
void CameraManager::RegisterSubCameras(std::vector<std::unique_ptr<Camera>>&& cameras, const std::string& prefix) {
    subCamerasMap_.clear();    // 既存のサブカメラは破棄

    int idx = 0;
    for (std::unique_ptr<Camera>& cam : cameras) {
        std::string name = prefix + "_" + std::to_string(idx++);
        subCamerasMap_[name] = std::move(cam); // ムーブで登録
    }
}

// 各シーン用カメラの切替
void CameraManager::OnSceneChanged(SceneCameraType type) {
    activeSceneCameraType_ = type;
    // シーンタイプに応じたカメラを設定
    switch (type) {
    case SceneCameraType::Title:
        currentSceneCamera_ = titlecamera_.get();
        break;
    case SceneCameraType::GameOver:
        currentSceneCamera_ = gameovercamera_.get();
        break;
    }
    // シーン切替時に初期化を呼んでサブカメラを再生成
    if (currentSceneCamera_) {
        currentSceneCamera_->Initialize();
    }
    // imguiのサブカメラ選択をリセット
    Typeview_ = ViewCameraType::Main;  // メインカメラに戻す
    activeSubCameraName_.clear();
    sceneCameraJustChanged_ = true;
}

// シーンマネージャーから現在のシ―ンを判定する
void CameraManager::NotifySceneChangedByName(const std::string& sceneName) {
    SceneCameraType newType;

    if (sceneName == "TITLE")     newType = SceneCameraType::Title;
    else if (sceneName == "GAMEPLAY")  newType = SceneCameraType::Gameplay;
    else if (sceneName == "GAMECLEAR") newType = SceneCameraType::GameClear;
    else if (sceneName == "GAMEOVER")  newType = SceneCameraType::GameOver;
    else newType = SceneCameraType::Title;

    // 変更を検知しシーンが変わったら処理
    if (newType != activeSceneCameraType_) {
        sceneCameraJustChanged_ = true;
        lastSceneCameraType_ = activeSceneCameraType_;
        activeSceneCameraType_ = newType;
        OnSceneChanged(newType);// 各シーン用カメラの切替もここで反映する
    }
}