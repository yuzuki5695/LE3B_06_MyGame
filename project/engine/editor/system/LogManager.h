#pragma once
#include <vector>
#include <string>
#include <memory>
#include <EditorTypes.h>

/// ログの重要度レベル
/// </summary>
enum class LogLevel {
    Info,       /// <summary> 情報 </summary>
    Warning,    /// <summary> 警告 </summary>
    Error       /// <summary> エラー </summary>
};

/// <summary>
/// 1件分のログデータ構造体
/// </summary>
struct LogEntry {
    std::string message;    /// <summary> ログ本文 </summary>
    LogLevel level;         /// <summary> ログレベル </summary>
    float timestamp;        /// <summary> 発生時間(秒) </summary>
};
/// <summary>
/// ImGuiエディタ内のコンソールに表示するログを管理するマネージャ
/// </summary>
class LogManager {
private: // シングルトン
	static std::unique_ptr<LogManager> instance; // インスタンス

	LogManager(LogManager&) = delete;
	LogManager& operator=(LogManager&) = delete;
public: // メンバ関数
	LogManager() = default;
	~LogManager() = default;
	// シングルトンインスタンスの取得
	static LogManager* GetInstance();
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
    void AddLog(const std::string& message, LogLevel level = LogLevel::Info);
    /// <summary>
    /// 多言語対応キーからメッセージを取得してログに追加
    /// </summary>
    /// <param name="key">MessageServiceで使用するJSONキー</param>
    /// <param name="level">ログレベル（デフォルトはInfo）</param>
    void AddLocalizedLog(const std::string& key, LogLevel level = LogLevel::Info);

private: // メンバ変数
    std::vector<LogEntry> logs_; /// ログ履歴の格納用コンテナ
public: // アクセッサ
	/// Getter
    const std::vector<LogEntry> GetLogs() const { return logs_; }   /// 全てのログエントリーを取得
};