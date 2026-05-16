#include "SceneManager.h"
#include <cassert>
#include <EditorManager.h>
#include <EditorConsole.h>

namespace MyEngine {
	// 静的メンバ変数の定義
	std::unique_ptr<SceneManager> SceneManager::instance = nullptr;

	// シングルトンインスタンスの取得
	SceneManager* SceneManager::GetInstance() {
		if (!instance) {
			instance = std::make_unique<SceneManager>();
		}
		return instance.get();
	}

	// 終了
	void SceneManager::Finalize() {
		// 最後のシーンの終了と解放
		if (scene_) {
			scene_->Finalize();
			scene_ = nullptr;
		}
		// シーンマネージャのインスタンスの解放
		instance.reset();
	}

	void SceneManager::Update() {
		// TODO:シーンの切り替え機構

		// 次のシーンの予約があるなら
		if (nextScene_) {
#ifdef USE_IMGUI
			// 現在シーン名を保存
			std::string oldSceneName;
			// scene_ が存在する時だけ取得
			bool hasOldScene = false;
			if (scene_) {
				// 旧シーン名を保存
				oldSceneName = scene_->GetSceneName();
				hasOldScene = true; // フラグを立てる
			}
			// シーン切り替え通知
			EditorManager::GetInstance()->OnSceneChanged();
#endif // USE_IMGUI

			// 旧シーンの終了
			if (scene_) {
				scene_->Finalize();
			}

			// シーン切り替え
			scene_ = std::move(nextScene_); // 所有権を移動
			nextScene_ = nullptr;
			// シーンマネージャをセット 
			scene_->SetSceneManager(this);
			// 次のシーンを初期化する
			scene_->Initialize();

#ifdef USE_IMGUI
			// 初回以外のみログを出す
			if (hasOldScene) {
				// 新シーン名を保存
				std::string newSceneName = scene_->GetSceneName();
				// 多言語対応されたシーン名を取得
				std::string oldLocalized = MessageService::GetText("Scene." + oldSceneName);
				// 多言語対応されたシーン名を取得
				std::string newLocalized = MessageService::GetText("Scene." + newSceneName);
				// ログ生成
				std::string log = MessageService::GetText("Scene.Changed");
				// ログ内のプレースホルダを置換
				size_t pos0 = log.find("{0}");
				if (pos0 != std::string::npos) { log.replace(pos0, 3, oldLocalized); }
				// ログ内のプレースホルダを置換
				size_t pos1 = log.find("{1}");
				if (pos1 != std::string::npos) { log.replace(pos1, 3, newLocalized); }
				// ログ出力
				EditorConsole::GetInstance()->AddLog(log);
			}
#endif
		}
		// 実行中シーンを更新する
		scene_->Update();
	}

	void SceneManager::Draw() {
		scene_->Draw();
	}

	void SceneManager::ChangeScene(const std::string& sceneName) {
		assert(sceneFactory_ && nextScene_ == nullptr);
		// 次のシーン生成
		nextScene_ = std::unique_ptr<BaseScene>(sceneFactory_->CreateScene(sceneName));
	}
}