#pragma once
#include <BaseScene.h>
#include <AbstractSceneFactory.h>
#include <memory>
#include <unordered_map>

namespace MyEngine {
	// シーン管理
	class SceneManager {
	private: // シングルトンインスタンス
		static std::unique_ptr<SceneManager> instance_;

		SceneManager(SceneManager&) = delete;
		SceneManager& operator=(SceneManager&) = delete;
	public: // メンバ関数
		SceneManager() = default;
		~SceneManager() = default;
		// シングルトンインスタンスの取得
		static SceneManager* GetInstance();		
		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// 毎フレームの更新処理
		/// </summary>
		void Update();
		/// <summary>
		/// 毎フレームの描画処理
		/// </summary>
		void Draw();
	private: // メンバ変数	
		// 今のシーン(実行中シーン)
		std::unique_ptr<BaseScene> scene_ = nullptr;
		// 次のシーン
		std::unique_ptr<BaseScene> nextScene_ = nullptr;
		// シーンファクトリー(借りてくる)
		AbstractSceneFactory* sceneFactory_ = nullptr;
	public:
		/// <summary>
		/// 次シーン予約
		/// </summary>
		/// <param name="sceneName">シーン名</param>
		void ChangeScene(const std::string& sceneName);
		// シーンファクトリーにsetter
		void SetSceneFactory(AbstractSceneFactory* SceneFactory) { sceneFactory_ = SceneFactory; }
		// 現在のシーンを取得する
		BaseScene* GetCurrentScene() const { return scene_.get(); }
		// 現在のシーンか判定
		bool IsCurrentScene(const std::string& sceneName) const { return scene_ && scene_->GetSceneName() == sceneName; }
		// 現在のシーン名を取得する
		const std::string& GetCurrentSceneName() const { return scene_->GetSceneName(); }
		// シーンファクトリーを取得する
		AbstractSceneFactory* GetSceneFactory() const { return sceneFactory_; }	
	};
}