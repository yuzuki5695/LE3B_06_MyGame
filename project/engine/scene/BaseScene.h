#pragma once
#include <string>

namespace MyEngine {
	// 前方宣言 
	class SceneManager;

	// シーン基底クラス
	class BaseScene
	{
	public:// メンバ関数
		// デストラクタ
		virtual ~BaseScene() = default;

		// 初期化
		virtual void Initialize() = 0;
		// 終了
		virtual void Finalize() = 0;
		// 毎フレーム更新
		virtual void Update() = 0;
		// 描画
		virtual void Draw() = 0;
	private:
		// シーンマネージャ（借りてくる）
		SceneManager* sceneManager_ = nullptr;
		// シーン名
		std::string sceneName_;
	public:// メンバ関数
		virtual void SetSceneManager(SceneManager* sceneManeger) { sceneManager_ = sceneManeger; }
		SceneManager* GetSceneManager() { return sceneManager_; }
		void SetSceneName(const std::string& name) { sceneName_ = name; }
		const std::string& GetSceneName() const { return sceneName_; }
	};
}