#include "SceneFactory.h"
#include <SceneName.h>
#include <TitleScene.h>
#include <GamePlayScene.h>
#include <GameClearScene.h>
#include <GameOverScene.h>	

using namespace MyGame;

namespace MyEngine {

	// コンストラクタで各シーンを登録
	SceneFactory::SceneFactory() { RegisterScenes(); }

	void SceneFactory::RegisterScenes() {
		// 二重登録しないようにガードを入れる 
		if (!sceneRegistry_.empty()) return;
		// ラムダ式を使用して各シーンの生成処理を登録
		sceneRegistry_[SceneName::TITLE] = []() { return new TitleScene(); };
		sceneRegistry_[SceneName::GAMEPLAY] = []() { return new GamePlayScene(); };
		sceneRegistry_[SceneName::GAMECLEAR] = []() { return new GameClearScene(); };
		sceneRegistry_[SceneName::GAMEOVER] = []() { return new GameOverScene(); };
	}

	BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {
		// マップ検索で捜す
		std::unordered_map<std::string, std::function<BaseScene* ()>>::iterator it = sceneRegistry_.find(sceneName);
		if (it != sceneRegistry_.end()) {
			BaseScene* newScene = it->second(); // 登録された関数を実行
			newScene->SetSceneName(sceneName);  // シーン名をセット
			return newScene;
		}
		return nullptr;
	}
	std::vector<std::string> SceneFactory::GetSceneNames() const {
		std::vector<std::string> names;
		// マップのキーをすべて取得してリストにする
		for (const auto& scene : sceneRegistry_) {
			names.push_back(scene.first); // シーン名をリストに追加
		}
		return names;
	}
}