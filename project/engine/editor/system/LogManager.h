#pragma once
#include <vector>
#include <string>
#include <memory>
#include <EditorTypes.h>

/// <summary>
/// エディタ内のコンソールに表示するログを管理するマネージャ
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
    /// ログメッセージの追加
    /// </summary>
    /// <param name="message">表示したい文字列</param>
    void AddLog(const std::string& message);
    void AddLog(Editor::MessageId id);       // オーバーロード
    /// <summary>
    /// 全てのログ履歴を消去する
    /// </summary>
    void Clear();
private: // メンバ変数
    std::vector<std::string> logs_; // ログ文字列の履歴
public: // アクセッサ
	/// Getter
    const std::vector<std::string>& GetLogs() const { return logs_; }   /// ログ配列の参照を取得
};