#include "CameraManager.h"
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <MatrixVector.h>
#include <SceneName.h>

using namespace MyGame;

namespace MyEngine {

    using namespace MatrixVector;
    using namespace CameraDefs;

    // カメラレジストリの型定義
    using CameraCreator = std::function<std::unique_ptr<ISceneCameraBehavior>()>;
    using CameraRegistry = std::unordered_map<std::string, CameraCreator>;

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

    void CameraManager::Initialize(const std::string& sceneName) {
        // メインカメラの生成と初期化
        camera_.mainCamera = std::make_unique<Camera>();
        // デフォルトはメインカメラをアクティブにしておく
        camera_.activeCamera = camera_.mainCamera.get();
        // 各シーンごとのカメラクラス挙動の登録
        RegisterCamera();
        // 各シーンカメラの初期化処理を呼び出す
        OnSceneChanged(sceneName);
    }
 
    void CameraManager::Update() {
        if (!camera_.activeCamera) return;

        // 現在の挙動を更新させる
        if (currentBehavior_) {
            currentBehavior_->Update(camera_.activeCamera);
        }

		// メインカメラの更新
        if (camera_.mainCamera) {
            camera_.mainCamera->Update();
        }

		// サブカメラ群の更新
        for (auto& [name, cam] : camera_.subCameras) {
            if (cam) {
                cam->Update();
            }
        }
    }

    void CameraManager::SetSceneBehavior(std::unique_ptr<MyGame::ISceneCameraBehavior> behavior) {
        // 古い挙動を所有権ごと破棄し、新しい挙動へ移行
        currentBehavior_ = std::move(behavior);
        // 移行直後の初期化を実行
        if (currentBehavior_) {
            currentBehavior_->Initialize(camera_.activeCamera);
        }
    }
    
    void CameraManager::RegisterCamera() {
        // 二重登録防止
        if (!cameraRegistry_.empty()) return;
        // 各シーンの登録
        cameraRegistry_[SceneName::TITLE] = [] {return std::make_unique<MyGame::TitleCamera>(); };
        cameraRegistry_[SceneName::GAMEPLAY] = [] {return std::make_unique<MyGame::GamePlayCamera>(); };
        cameraRegistry_[SceneName::GAMEOVER] = [] {return std::make_unique<MyGame::GameOverCamera>(); };
        cameraRegistry_[SceneName::GAMECLEAR] = [] {return std::make_unique<MyGame::GameClearCamera>(); };
    }

    void CameraManager::OnSceneChanged(const std::string& sceneName) {
        // レジストリからシーン名に合致する生成関数を検索
        CameraRegistry::iterator it = cameraRegistry_.find(sceneName);
        if (it != cameraRegistry_.end()) {
            // 生成関数を呼び出して新しい挙動をセット
            SetSceneBehavior(it->second());
        }
    }
}