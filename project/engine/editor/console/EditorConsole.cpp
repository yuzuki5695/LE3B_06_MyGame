#include "EditorConsole.h"
#include <__msvc_chrono.hpp>
#include <format>
#include <MessageService.h>
#include <TimeSystem.h>

// 静的メンバ変数の定義
std::unique_ptr<EditorConsole> EditorConsole::instance = nullptr;

// シングルトンインスタンスの取得
EditorConsole* EditorConsole::GetInstance() {
    if (!instance) {
        instance = std::make_unique<EditorConsole>();
    }
    return instance.get();
}

// 終了
void EditorConsole::Finalize() {
    instance.reset();  // `delete` 不要
}

void EditorConsole::Clear() {
    // ログ履歴の全消去
    logs_.clear();
}

void EditorConsole::AddLog(const std::string& message, LogLevel level) {
    float elapsed = static_cast<float>(TimeSystem::GetElapsedSeconds());
    logs_.push_back({ message, level, elapsed });
}

void EditorConsole::AddLocalizedLog(const std::string& key, LogLevel level) {
    // MessageServiceから翻訳テキストを取得
    std::string message = MessageService::GetText(key);
    // 実際の追加処理
    AddLog(message, level);
}