#include "LogManager.h"
#include <__msvc_chrono.hpp>
#include <format>
#include <MessageService.h>
#include <TimeSystem.h>

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

void LogManager::Clear() {
    // ログ履歴の全消去
    logs_.clear();
}

void LogManager::AddLog(const std::string& message, LogLevel level) {
    float elapsed = static_cast<float>(TimeSystem::GetElapsedSeconds());
    logs_.push_back({ message, level, elapsed });
}

void LogManager::AddLocalizedLog(const std::string& key, LogLevel level) {
    // MessageServiceから翻訳テキストを取得
    std::string message = MessageService::GetText(key);
    // 実際の追加処理
    AddLog(message, level);
}