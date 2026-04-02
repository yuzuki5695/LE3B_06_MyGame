#pragma once
#include <vector>
#include <memory>
#include <EditorTypes.h>
#include <EditorConsoleTypes.h>

namespace MyEngine {
    /// <summary>
    /// ImGuiエディタ内のコンソールに表示するログを管理するマネージャ
    /// </summary>
    class EditorConsole {
    private: // シングルトン
        static std::unique_ptr<EditorConsole> instance; // インスタンス

        EditorConsole(EditorConsole&) = delete;
        EditorConsole& operator=(EditorConsole&) = delete;
    public: // メンバ関数
        EditorConsole() = default;
        ~EditorConsole() = default;
        // シングルトンインスタンスの取得
        static EditorConsole* GetInstance();
        // 終了
        void Finalize();
        /// <summary>
        /// 全てのログ履歴を消去する
        /// </summary>
        void Clear();
        /// <summary>
        /// ログメッセージの追加
        /// </summary>
        /// <param name="message">表示したい文字列</param>
        /// <param name="level">ログレベル（デフォルトはInfo）</param> 
        void AddLog(const std::string& message, Editor::console::LogLevel level = Editor::console::LogLevel::Info);
        /// <summary>
        /// 多言語対応キーからメッセージを取得してログに追加
        /// </summary>
        /// <param name="key">MessageServiceで使用するJSONキー</param>
        /// <param name="level">ログレベル（デフォルトはInfo）</param>
        void AddLocalizedLog(const std::string& key, Editor::console::LogLevel level = Editor::console::LogLevel::Info);

    private: // メンバ変数
        std::vector<Editor::console::LogEntry> logs_; /// ログ履歴の格納用コンテナ
    public: // アクセッサ
        /// Getter
        const std::vector<Editor::console::LogEntry> GetLogs() const { return logs_; }   /// 全てのログエントリーを取得
    };
}