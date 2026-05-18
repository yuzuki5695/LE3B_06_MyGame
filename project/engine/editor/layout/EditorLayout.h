#pragma once
#include <vector>
#include <memory>
#include <IEditorWindow.h>
#include <SrvManager.h>
#include <EditorMenuBar.h>
#include <EditorTypes.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI

namespace MyEngine {
    /// <summary>
    /// エディタ全体のレイアウト・外観を管理するクラス
    /// メニューバーやパネルの配置など、画面の枠組みを構築する。
    /// </summary>
    class EditorLayout {
    private:
        // 左側のパネルサイズ
        static constexpr float kGameViewWidth_ = 640.0f;
        static constexpr float kGameViewHeight_ = 360.0f;
        // 右側のパネルサイズ
        static constexpr float kRightPanelViewWidth_ = 640.0f;
    public: // メンバ関数
        /// <summary>
        /// 初期化処理
        /// </summary>
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
        /// <summary>
        /// ゲームビューを全画面表示する
        /// </summary>
        /// <param name="srvmanager"></param>
        void DrawFullscreenGameView(SrvManager* srvmanager);
#ifdef USE_IMGUI
        /// <summary>
        /// ドッキングされたウィンドウを描画するための共通関数
        /// </summary>
        /// <typeparam name="WindowType"></typeparam>
        /// <param name="window"></param>
        /// <param name="dockspace_id"></param>
        /// <returns></returns>
        template<class WindowType>
        bool BeginDockedWindow(WindowType& window, ImGuiID dockspace_id) {
            ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
            bool is_open = true;
            if (ImGui::Begin(window.name.c_str(), &is_open, ImGuiWindowFlags_NoCollapse)) {
                if (window.drawEditor) {
                    window.drawEditor();
                }
            }
            ImGui::End();
            return is_open;
        }
#endif // USE_IMGUI

    private: // メンバ変数
        std::function<std::string(const std::string&)> LT;  /// 多言語テキスト取得用の短縮ラムダ
        std::unique_ptr<EditorMenuBar> menuBar_;            /// メニューバー
        bool requestResetLayout_ = true;                    /// レイアウトリセット要求フラグ
        void* activeObject_ = nullptr;                      /// 選択中オブジェクトのポインタ
        std::string activeObjectName_;                      /// 選択中オブジェクトの名前
        EditorTypes::EditorObjectInfo activeCategory_;      /// 型情報を保持して判定用
        bool isGameViewFullscreen_ = false;                 /// GameView最大化状態
    public:
        // getter
        EditorMenuBar* GetMenuBar() { return menuBar_.get(); }
        // setter
        void SetGameViewFullscreen(bool fullscreen) { isGameViewFullscreen_ = fullscreen; }

        bool IsGameViewFullscreen() const { return isGameViewFullscreen_; }
        void ToggleGameViewFullscreen() { isGameViewFullscreen_ = !isGameViewFullscreen_; }
    };
}