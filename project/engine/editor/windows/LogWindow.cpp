#include "LogWindow.h"
#include <ImGuiManager.h>
#include <LogManager.h>

LogWindow::LogWindow() {
    isOpen = true; // デフォルトで表示
}

void LogWindow::Draw() {
    //// ウィンドウの開始
    //if (!ImGui::Begin(GetName(), &isOpen)) {
    //    ImGui::End();
    //    return;
    //}

    //// ログを消去するボタン
    //if (ImGui::Button("Clear")) {
    //    LogManager::GetInstance()->Clear();
    //}
    //ImGui::SameLine();
    //ImGui::Checkbox("Auto-scroll", &autoScroll_);

    //ImGui::Separator();

    //// ログ表示領域（子ウィンドウでスクロール可能にする）
    //const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    //ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

    //// LogManagerからログを取得して描画
    //const auto& logs = LogManager::GetInstance()->GetLogs();
    //for (const auto& log : logs) {
    //    ImGui::TextUnformatted(log.c_str());
    //}

    //// 自動スクロール処理
    //if (autoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    //    ImGui::SetScrollHereY(1.0f);
    //}

    //ImGui::EndChild();
    //ImGui::End();
}