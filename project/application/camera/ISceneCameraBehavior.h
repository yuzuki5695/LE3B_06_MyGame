#pragma once
#include <Camera.h>
#include <Object3d.h>

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


        virtual void SetTarget(MyEngine::Object3d* target) {}
    };
}