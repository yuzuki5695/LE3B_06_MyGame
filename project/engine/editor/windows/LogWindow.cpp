#include "LogWindow.h"
#include <ImGuiManager.h>
#include <LogManager.h>
#include <MessageService.h>

LogWindow::LogWindow() {
    isOpen = true; // デフォルトで表示
}

void LogWindow::Draw() {
   // MessageServiceからテキストを取得
    std::string clearText = MessageService::GetText("Console.Clear");

    // ログを消去するボタン
    if (ImGui::Button(clearText.c_str())) {
        LogManager::GetInstance()->Clear();
    }

    ImGui::Separator();
    
    // ログ表示領域（子ウィンドウでスクロール可能にする）
    // 第二引数に 0 を指定すると、親の残りのスペースをすべて使います
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    const auto& logs = LogManager::GetInstance()->GetLogs();
    for (const auto& log : logs) {
        std::string formatted = std::format("[{:.3f}] {}", log.timestamp, log.message);

        switch (log.level)
        {
        case LogLevel::Error:
            ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), formatted.c_str());
            break;

        case LogLevel::Warning:
            ImGui::TextColored(ImVec4(1, 1, 0.4f, 1), formatted.c_str());
            break;

        default:
            ImGui::TextUnformatted(formatted.c_str());
            break;
        }
    }
    // 自動スクロール処理
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}