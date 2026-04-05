#include "CameraManager.h"
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <MatrixVector.h>
#include <SceneName.h>

namespace MyEngine {

    using namespace MyGame;

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

    void CameraManager::Initialize(const Transform& Transform) {
        // メインカメラの生成と初期姿勢の反映
        camera_.mainCamera = std::make_unique<Camera>();
        camera_.mainCamera->SetTranslate(Transform.translate);
        camera_.mainCamera->SetRotate(Transform.rotate);
        camera_.activeCamera = camera_.mainCamera.get(); // 初期状態ではメインカメラを操作対象とする

        // 利用可能なシーンカメラクラスの登録
        RegisterCamera();
    }

    void CameraManager::Update() {
        if (!camera_.activeCamera) {
            return; // アクティブカメラが存在しない場合は更新処理をスキップ
        }
        
        // 現在の挙動をアクティブカメラに適用
        if (currentBehavior_) {
            currentBehavior_->Update(camera_.activeCamera);
        }

        // マネージャ側の管理用Transformに最新情報を同期
        camera_.transform.translate = camera_.activeCamera->GetTranslate();
        camera_.transform.rotate = camera_.activeCamera->GetRotate();

        // 最終的な行列の計算
        camera_.activeCamera->Update();
    }

    void CameraManager::SetSceneBehavior(std::unique_ptr<MyGame::ISceneCameraBehavior> behavior) {
        // 古い挙動を所有権ごと破棄し、新しい挙動へ移行
        currentBehavior_ = std::move(behavior);

        // 移行直後の初期化を実行
        if (currentBehavior_) {
            currentBehavior_->Initialize(camera_.GetActive());
        }
    }

    void CameraManager::OnSceneChanged(const std::string& sceneName) {
        // レジストリからシーン名に合致する生成関数を検索
        CameraRegistry::iterator it = cameraRegistry_.find(sceneName);
        if (it != cameraRegistry_.end()) {
            // 生成関数を呼び出して新しい挙動をセット
            SetSceneBehavior(it->second());
        }
    }

    void CameraManager::RegisterCamera() {
        // 二重登録防止
        if (!cameraRegistry_.empty()) return;
        // 各シーンの登録
        cameraRegistry_[SceneName::TITLE] = [] {return std::make_unique<MyGame::TitleCamera>(); };
        cameraRegistry_[SceneName::GAMEPLAY] = [] {return std::make_unique<MyGame::GamePlayCamera>(); };
    }
}