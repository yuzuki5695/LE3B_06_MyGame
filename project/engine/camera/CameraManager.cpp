#include "CameraManager.h"
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <MatrixVector.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI

namespace MyEngine {

    using namespace MatrixVector; 
    using namespace CameraTypes;

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
    }

    // 更新処理
    void CameraManager::Update() {
        // シーンカメラが変わったら一度だけTransformとサブカメラを反映
        //if (sceneCameraJustChanged_) {
        //    // 前のサブカメラを破棄
        //    subCamerasMap_.clear();
        //    // 新しいシーンに切り替えるときのみムーブ
        //    if (currentSceneCamera_) { RegisterSubCameras(std::move(currentSceneCamera_->MoveSubCameras()), "SubCamera"); }
        //    sceneCameraJustChanged_ = false;
        //}

        //// 現在のシーンカメラの更新
        //if (currentSceneCamera_) { currentSceneCamera_->Update(); }

        //// メインカメラに値を反映
        //if (mainCamera_ && currentSceneCamera_) {
        //    maintrans_ = currentSceneCamera_->GetMainTransform();
        //    mainCamera_->SetTranslate(maintrans_.translate);
        //    mainCamera_->SetRotate(maintrans_.rotate);
        //    mainCamera_->Update();
        //}

        //// サブカメラ更新
        //for (std::pair<const std::string, std::unique_ptr<Camera>>& subcameras : subCamerasMap_) {
        //    subcameras.second->Update();
        //}
        //// アクティブ登録
        //SetActiveCamera();
    }

    // アクティブカメラを取得
    Camera* CameraManager::GetActiveCamera() {
        //// Transition中は常にメインカメラを返す
        //if (currentMode_ == CameraMode::Transition) {
        //    return mainCamera_.get();
        //}

        //switch (Typeview_) {
        //case ViewCameraType::Main:
        //    return mainCamera_.get();
        //case ViewCameraType::Sub:
        //    if (!activeSubCameraName_.empty() && subCamerasMap_.count(activeSubCameraName_)) {
        //        return subCamerasMap_[activeSubCameraName_].get();
        //    }
        //    else if (!subCamerasMap_.empty()) {
        //        return subCamerasMap_.begin()->second.get(); // 最初のサブカメラ
        //    }
        //    break;
        //}
        return mainCamera_.get();
    }
}