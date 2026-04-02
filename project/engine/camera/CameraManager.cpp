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
    
    }

    // アクティブカメラを取得
    Camera* CameraManager::GetActiveCamera() {
        return mainCamera_.get();
    }
}