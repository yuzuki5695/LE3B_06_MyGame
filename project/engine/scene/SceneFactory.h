#pragma once
#include <AbstractSceneFactory.h>
#include <unordered_map>
#include <functional>

namespace MyEngine {
	// このゲーム用のシーン工場
	class SceneFactory : public AbstractSceneFactory {
	public: // メンバ関数
		//	コンストラクタ
		SceneFactory();

		// 初期化時にシーンを登録する関数
		void RegisterScenes();

		/// <summary>
		/// シーン生成
		/// </summary>
		/// <param name="sceneName">シーン名</param>
		/// <returns>生成したシーン</returns>
		virtual BaseScene* CreateScene(const std::string& sceneName) override;
	private: // メンバ変数
		// シーン名と生成関数のマップ
		std::unordered_map<std::string, std::function<BaseScene* ()>> sceneRegistry_;
	};
}