#pragma once
#include <BaseScene.h>
#include <AbstractSceneFactory.h>
#include <memory>
#include <unordered_map>

namespace MyEngine {
	// シーン管理
	class SceneManager {
	public: // メンバ関数
		// シーンマネージャの状態
		enum class State {
			Idle,        // 通常実行中
			FadeOut,     // シーン終了演出中
			Change,      // シーン差し替え実行
			FadeIn       // 次のシーン開始演出中
		};
	private:
		static std::unique_ptr<SceneManager> instance;

		SceneManager(SceneManager&) = delete;
		SceneManager& operator=(SceneManager&) = delete;
	public: // メンバ関数
		SceneManager() = default;
		~SceneManager() = default;

		// シングルトンインスタンスの取得
		static SceneManager* GetInstance();
		// 終了
		void Finalize();
		// 毎フレーム更新
		void Update();
		// 描画
		void Draw();
	private: // メンバ変数	
		//State state_ = State::Idle; // 現在の状態
		// 今のシーン(実行中シーン)
		std::unique_ptr<BaseScene> scene_ = nullptr;
		// 次のシーン
		std::unique_ptr<BaseScene> nextScene_ = nullptr;
		// シーンファクトリー(借りてくる)
		AbstractSceneFactory* sceneFactory_ = nullptr;
		// シーン到達フラグのマップ
		std::unordered_map<std::string, bool> sceneReachedFlags_;
	public:
		/// <summary>
		/// 次シーン予約
		/// </summary>
		/// <param name="sceneName">シーン名</param>
		void ChangeScene(const std::string& sceneName);
		// シーンファクトリーにsetter
		void SetSceneFactory(AbstractSceneFactory* SceneFactory) { sceneFactory_ = SceneFactory; }
		// シーン到達フラグを設定
		void SetSceneReached(const std::string& sceneName, bool reached = true) {
			sceneReachedFlags_[sceneName] = reached;
		}

		// シーンに一度でも到達したか？
		bool HasReachedScene(const std::string& sceneName) const {
			auto it = sceneReachedFlags_.find(sceneName);
			return it != sceneReachedFlags_.end() && it->second;
		}

		// 現在のシーンを取得する getter
		BaseScene* GetCurrentScene() const {
			return scene_.get();
		}
		/// <summary>
		/// 現在のシーン名が指定した名前と一致するか判定する
		/// </summary>
		/// <param name="sceneName">比較したいシーン名</param>
		/// <returns>一致していればtrue</returns>
		bool IsCurrentScene(const std::string& sceneName) const {
			// 現在のシーンが存在し、かつ名前が一致しているか
			if (scene_) {
				return scene_->GetSceneName() == sceneName;
			}
			return false;
		}
		// 現在のシーンが「到達済み」フラグを持っているか？
		bool IsCurrentSceneReached() const {
			if (!scene_) { return false; }

			// BaseScene 側でシーン名を持っている前提
			const std::string& currentSceneName = scene_->GetSceneName();

			auto it = sceneReachedFlags_.find(currentSceneName);
			return it != sceneReachedFlags_.end() && it->second;
		}
	};
}