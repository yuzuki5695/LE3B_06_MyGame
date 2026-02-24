#include "LogManager.h"
#include <__msvc_chrono.hpp>
#include <format>
#include <MessageService.h>

// 静的メンバ変数の定義
std::unique_ptr<LogManager> LogManager::instance = nullptr;

// シングルトンインスタンスの取得
LogManager* LogManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<LogManager>();
    }
    return instance.get();
}

// 終了
void LogManager::Finalize() {
    instance.reset();  // `delete` 不要
}

void LogManager::AddLog(const std::string& message) {
    // 静的変数で開始時間を記録
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    // 経過時間を秒単位で計算
    float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() / 1000.0f;
    // [0.123] メッセージ の形式で整形
    std::string formattedLog = std::format("[{:.3f}] {}", elapsed, message);
    // ログ履歴に追加
    logs_.push_back(formattedLog);
}

void LogManager::AddLog(Editor::MessageId id) {
    // MessageServiceクラスを利用して、現在の言語設定に応じた文字列を取得
    std::string message = MessageService::FetchMessage(id);

    // 文字列版の AddLog を呼び出して、実際のログ記録を行う
    AddLog(message);
}

void LogManager::Clear() {
    // ログ履歴の全消去
    logs_.clear();
}