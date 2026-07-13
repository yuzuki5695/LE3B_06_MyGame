#pragma once
#include <BaseScene.h>
#include <Skybox.h>
#include <Object3d.h>

namespace MyGame {
    /// <summary>
    /// ステージマネージャ
    /// </summary>
    class StageManager {
	private: // シングルトンインスタンス
        static std::unique_ptr<StageManager> instance_;

        StageManager(StageManager&) = delete;
        StageManager& operator=(StageManager&) = delete;
    public: // メンバ関数
        StageManager() = default;
        ~StageManager() = default;
        // シングルトンインスタンスの取得
        static StageManager* GetInstance();
        // 終了
        void Finalize();
        /// <summary>
		/// 初期化
        /// </summary>
        void Initialize();
        /// <summary>
		/// 毎フレーム更新
        /// </summary>
        void Update();
        /// <summary>
		/// 描画
        /// </summary>
        void Draw();
        /// <summary>
		/// DDS描画
        /// </summary>
        void DDSDraw();
    private: // メンバ変数
        // 地面
        std::unique_ptr <MyEngine::Object3d> grass = nullptr;
		// スカイボックス
        std::unique_ptr<MyEngine::Skybox> skybox_;
		// クリアゲート
        std::unique_ptr <MyEngine::Object3d> clearwall_;    /// クリアゲート
    public:
        void SetClearwallTranslate(const MyEngine::Vector3& translate) {
            if (clearwall_) { clearwall_->SetTranslate(translate); }
        }
    };
}