#pragma once
#include <vector>
#include <memory>
#include <IEditorWindow.h>
#include <SrvManager.h>
#include <MessageService.h>
#include <EditorLayout.h>

namespace MyEngine {
	/// <summary>
	/// IMGUIエディタ全体の管理
	/// </summary>
	class EditorManager {
	private: // シングルトン
		static std::unique_ptr<EditorManager> instance_; // インスタンス

		EditorManager(EditorManager&) = delete;
		EditorManager& operator=(EditorManager&) = delete;
	public: // メンバ関数
		EditorManager() = default;
		~EditorManager() = default;
		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		static EditorManager* GetInstance();
		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();
		/// <summary>
		/// 全体の更新と描画
		/// </summary>
		void Draw(SrvManager* srvmanager);

		/// <summary>
		/// 任意の文字列をログに出力する
		/// </summary>
		void Log(const std::string& message);

		/// <summary>
		/// ウィンドウの登録
		/// </summary>
		void AddWindow(std::unique_ptr<IEditorWindow> window);

		void SetLanguage(EditorTypes::Language lang);

		void OnSceneChanged();

	private:
		// 言語設定を列挙型で管理
		EditorTypes::Language language_ = EditorTypes::Language::Japanese;
		std::vector<std::unique_ptr<IEditorWindow>> windows_;
		bool showSettings_ = false; // 設定ウィンドウの表示フラグ
		// --- 追加：自動配置用 ---
		bool requestResetLayout_ = false; // レイアウトを強制的に組み直すフラグ
		std::unique_ptr<EditorLayout> layout_; // レイアウト担当を追加

	public: // アクセッサ	

	};
}
