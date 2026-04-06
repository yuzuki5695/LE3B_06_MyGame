#pragma once
#include <Object3d.h>
#include <CameraDefs.h>
#include <CameraManager.h>
#include <GamePlayCamera.h>
#include <CameraSet.h>
#include <TitleCamera.h>

namespace MyEngine {   
    /// <summary>     
    /// カメラマネージャクラス  
    /// </summary>
    class CameraManager {
    private:
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
        /// <param name="transform">初期カメラ姿勢</param>
        void Initialize(const Transform& Transform);
        /// <summary>
        /// 更新処理
        /// </summary> 
        void Update();
        /// <summary>
        /// 現在の挙動をセットする
        /// </summary>
        /// <param name="behavior">差し替える挙動の所有権</param>
        void SetSceneBehavior(std::unique_ptr<MyGame::ISceneCameraBehavior> behavior);
        /// <summary>
        /// 全挙動をレジストリに登録する（初期化時に呼ぶ）
        /// </summary>
        void RegisterCamera();
        /// <summary>
        /// シーン名に基づいて挙動を切り替える
        /// </summary>
        /// <param name="sceneName">切り替え先のシーン名</param>
        void OnSceneChanged(const std::string& sceneName);
    private: // メンバ変数
        // カメラのデータセット（
        CameraSet camera_;
        CameraDefs::StateData stateData_;
        // 現在適用中の挙動ロジック
        std::unique_ptr<MyGame::ISceneCameraBehavior> currentBehavior_;
        // レジストリ
        std::unordered_map<std::string, std::function<std::unique_ptr<MyGame::ISceneCameraBehavior>()>> cameraRegistry_;

    public: // アクセッサ
        // getter
        Camera* GetActiveCamera() const { return camera_.GetActive(); }
        CameraSet& GetCameraSet() { return camera_; }
        const CameraDefs::StateData& GetCameraState() const { return stateData_; }
        Object3d* GetTarget() { return stateData_.target; }

        // setter
        void SetCameraState(const CameraDefs::StateData& data) { stateData_ = data; }
        void SetTarget(Object3d* target) { stateData_.target = target; SetCameraState(stateData_); }
    };
}