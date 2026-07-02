#pragma once
#include <Object3d.h>
#include <ISceneCameraBehavior.h>
#include <Player.h>
#include <TitleCamera.h>
#include <GamePlayCamera.h>
#include <GameOverCamera.h>
#include <GameClearCamera.h>

namespace MyEngine {
    /// <summary>     
    /// カメラマネージャクラス  
    /// </summary>
    class CameraManager {
    private: 
		// シングルトンインスタンス
        static std::unique_ptr<CameraManager> instance;

        CameraManager(CameraManager&) = delete;
        CameraManager& operator=(CameraManager&) = delete;
    public: // メンバ関数
        CameraManager() = default;
        ~CameraManager() = default;
        // シングルトンインスタンスの取得
        static CameraManager* GetInstance();
        // 終了
        void Finalize();
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize(const std::string& sceneName);
        /// <summary>
        /// 更新処理
        /// </summary> 
        void Update();
        /// <summary>
		/// ImGuiの描画処理
        /// </summary>
        void DrawImGui();
        /// <summary>
        /// 現在の挙動をセットする
        /// </summary>
        /// <param name="behavior">差し替える挙動の所有権</param>
        void SetSceneBehavior(std::unique_ptr<MyGame::ISceneCameraBehavior> behavior);
        // シーンごとに必要な「特定の挙動」を取得したい場合、
        // テンプレートを使うとキャスト処理を汎用化できます
        template <typename T>
        T* GetCurrentBehaviorAs() { return dynamic_cast<T*>(currentBehavior_.get()); }
	private: // 内部関数
		// 各シーンカメラの登録
        void RegisterCamera();
		// シーン切り替え時の初期化処理
        void OnSceneChanged(const std::string& sceneName);
    private: // メンバ変数
        // カメラのデータセット
        CameraSet camera_;
        // 現在適用中の挙動ロジック
        std::unique_ptr<MyGame::ISceneCameraBehavior> currentBehavior_;
        // レジストリ
        std::unordered_map<std::string, std::function<std::unique_ptr<MyGame::ISceneCameraBehavior>()>> cameraRegistry_;
    public: // アクセッサ
        // getter
        Camera* GetActiveCamera() const { return camera_.activeCamera; }
        CameraSet& GetCameraSet() { return camera_; }
        const CameraDefs::StateData& GetCameraState() const { return currentBehavior_->GetStateData(); }
        // setter
        void SetMainCamera() { camera_.activeCamera = camera_.mainCamera.get(); }
        void SetActiveSubCamera(const std::string& name) {
            auto it = camera_.subCameras.find(name);
            if (it != camera_.subCameras.end()) {
                camera_.activeCamera = it->second.get();
            }
        }
    };
}