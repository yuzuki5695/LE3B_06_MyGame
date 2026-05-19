#pragma once
#include <Camera.h>
#include <Object3d.h>
#include <CameraDefs.h>
#include <CameraSet.h>

namespace MyGame {
    /// <summary>
    /// 各シーンカメラ挙動インターフェース
    /// </summary>
    class ISceneCameraBehavior {
    public: // メンバ関数
        virtual ~ISceneCameraBehavior() = default;
        /// <summary>
        /// 初期化処理
        /// </summary>
        /// <param name="camera">操作対象のカメラポインタ</param>
        virtual void Initialize(MyEngine::Camera* camera) = 0;
        /// <summary>
        /// 毎フレーム更新
        /// </summary>
        /// <param name="camera">操作対象のカメラポインタ</param>
        virtual void Update(MyEngine::Camera* camera) = 0;
        /// <summary>
        /// 操作対象のオブジェクトをセットする関数
        /// </summary>
        /// <param name="target"></param>
        virtual void SetTargetObject(MyEngine::Object3d* target) { target_ = target; }
    protected: // メンバ変数
        // 対象のオブジェクト
        MyEngine::Object3d* target_{};
        // カメラの状態データ
        MyEngine::CameraDefs::StateData stateData_;
    public:
        // getter
        const MyEngine::CameraDefs::StateData& GetStateData() const { return stateData_; }
        // setter
        void SetCameraState(const MyEngine::CameraDefs::CameraState& state) { stateData_.state = state; }
    };
}