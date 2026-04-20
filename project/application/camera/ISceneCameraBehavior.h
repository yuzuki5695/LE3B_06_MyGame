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
    public:
        virtual ~ISceneCameraBehavior() = default;

        /// <summary>
        /// 挙動の初期化（シーン切り替え時に一度呼ばれる）
        /// </summary>
        /// <param name="camera">操作対象のカメラポインタ</param>
        virtual void Initialize(MyEngine::Camera* camera) = 0;

        /// <summary>
        /// 挙動の毎フレーム更新
        /// </summary>
        /// <param name="camera">操作対象のカメラポインタ</param>
        virtual void Update(MyEngine::Camera* camera) = 0;

        // 対象のオブジェクトをセットする関数
        virtual void SetTargetObject(MyEngine::Object3d* target) { target_ = target; }

        // 進捗率(0.0~1.0)を返す仮想関数。デフォルトは0を返す
        virtual float GetProgress() const { return 0.0f; }
    protected:
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