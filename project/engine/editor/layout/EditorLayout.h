#pragma once
#include <vector>
#include <memory>
#include <IEditorWindow.h>
#include <SrvManager.h>
#include <EditorMenuBar.h>
#include <EditorTypes.h>

namespace MyEngine {
    /// <summary>
    /// エディタ全体のレイアウト・外観を管理するクラス
    /// メニューバーやパネルの配置など、画面の枠組みを構築する。
    /// </summary>
    class EditorLayout {
    public: // メンバ関数

        void Initialize();

        /// <summary>
        /// エディタ全体のレイアウトを描画するメイン関数
        /// </summary>
        /// <param name="srvmanager">SRV管理クラス</param>
        /// <param name="windows">表示対象のウィンドウリスト</param>
        void Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows);
    private:  // 内部関数a
        /// <summary>
        /// 上部メインメニューバーを表示する
        /// </summary>
        /// <param name="windows">ウィンドウリスト（表示切り替え用）</param>
        void ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows);
        /// <summary>
        /// 左側パネルを描画する（ゲームビューおよびシステムコンソール）
        /// </summary>
        /// <param name="srvmanager">GPUテクスチャハンドル取得用</param>
        void DrawLeftPanel(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows);
        /// <summary>
        /// 右側パネルを描画する
        /// </summary>
        void DrawRightPanel(std::vector<std::unique_ptr<IEditorWindow>>& windows);
    private: // メンバ変数
        std::function<std::string(const std::string&)> LT; /// 多言語テキスト取得用の短縮ラムダ
        std::unique_ptr<EditorMenuBar> menuBar_;                          /// メニューバー
        bool requestResetLayout_ = true;
        void* activeObject_ = nullptr;                     /// 選択中オブジェクトのポインタ
        std::string activeObjectName_;                     /// 選択中オブジェクトの名前
        EditorTypes::EditorObjectInfo activeCategory_;      /// 型情報を保持して判定用
    public:
        EditorMenuBar* GetMenuBar() { return menuBar_.get(); }
   
    };
}