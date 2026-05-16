#pragma once
#include <AbstractSceneFactory.h>
#include <unordered_map>
#include <functional>

namespace MyEngine {
	/// <summary>
	/// このゲーム用のシーン工場
	/// </summary>
	class SceneFactory : public AbstractSceneFactory {
	public: // メンバ関数
		//	コンストラクタ
		SceneFactory();
		/// <summary>
		/// シーン名と生成関数のマップにシーンを登録する
		/// </summary>
		void RegisterScenes();
		/// <summary>
		/// シーン生成
		/// </summary>
		/// <param name="sceneName">シーン名</param>
		/// <returns>生成したシーン</returns>
		virtual BaseScene* CreateScene(const std::string& sceneName) override;	
		/// <summary>
		/// シーン名のリストを取得
		/// </summary>
		/// <returns>シーン名のリスト</returns>
		std::vector<std::string> GetSceneNames() const override;
	private: // メンバ変数
		// シーン名と生成関数のマップ
		std::unordered_map<std::string, std::function<BaseScene* ()>> sceneRegistry_;
	};
}