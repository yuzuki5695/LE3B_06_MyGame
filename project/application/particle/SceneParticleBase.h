#pragma once
#include <memory>
#include <vector>
#include <BaseEmitter.h>

namespace MyGame {

    class SceneParticleBase {
    public:
        /// <summary>
		/// デストラクタ
        /// </summary>
        virtual ~SceneParticleBase() = default;
        /// <summary>
		/// 初期化処理
        /// </summary>
        virtual void Initialize() = 0;
        /// <summary>
		/// 更新処理
        /// </summary>
        virtual void Update() = 0;
        // どのシーン用のUIか設定・取得
        void SetTargetSceneName(const std::string& name) { sceneName_ = name; }
        // どのシーン用のUIか取得
        const std::string& GetTargetSceneName() const { return sceneName_; }
    protected: // メンバ変数
        std::string sceneName_; //  シーン名
    };
}