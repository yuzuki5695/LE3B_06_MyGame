#pragma once
#include <string>

namespace Editor::console {
    // ロゴの重要度レベル
     enum class LogLevel {
        Info,       ///< 情報
        Warning,    ///< 警告
        Error       ///< エラー
    };
    // 1件分のログデータ構造体
    struct LogEntry {
        std::string message;  ///< ログ本文
        LogLevel level;       ///< ログレベル
        float timestamp;      ///< 発生時間(秒)
    };
}