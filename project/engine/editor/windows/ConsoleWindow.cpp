#include "ConsoleWindow.h"
#include <ImGuiManager.h>
#include <EditorConsole.h>
#include <MessageService.h>

using namespace Editor::console;

ConsoleWindow::ConsoleWindow() {
    isOpen = true; // 初期表示ON
}

void ConsoleWindow::Draw() {
#ifdef USE_IMGUI
    //----------------------------------------------------
    // 1. 「Clear」ボタンの表示
    //----------------------------------------------------

    // ローカライズ対応：ボタンテキスト取得
    std::string clearText = MessageService::GetText("Console.Clear");

    // ボタン押下でログを全消去
    if (ImGui::Button(clearText.c_str())) {
        EditorConsole::GetInstance()->Clear();
    }

    ImGui::Separator(); // 区切り線

    //----------------------------------------------------
    // 2. ログ表示エリア（スクロール可能領域）
    //----------------------------------------------------

    // 子ウィンドウを使用してスクロール対応にする
    // ImVec2(0, 0) は「親ウィンドウの残り全領域を使用」
    ImGui::BeginChild(
        "ScrollingRegion",
        ImVec2(0, 0),
        false,
        ImGuiWindowFlags_HorizontalScrollbar
    );

    //----------------------------------------------------
    // 3. ログ一覧を取得
    //----------------------------------------------------
    const auto& logs = EditorConsole::GetInstance()->GetLogs();

    //----------------------------------------------------
    // 4. ログを1件ずつ描画
    //----------------------------------------------------
    for (const auto& log : logs) {

        // 表示用フォーマット
        // [経過秒] メッセージ
        std::string formatted = std::format("[{:.3f}] {}", log.timestamp, log.message);

        // ログレベルごとに色分け表示
        switch (log.level)
        {
        case LogLevel::Error:
            // 赤系（エラー）
            ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), formatted.c_str());
            break;

        case LogLevel::Warning:
            // 黄色系（警告）
            ImGui::TextColored(ImVec4(1, 1, 0.4f, 1), formatted.c_str());
            break;

        default:
            // 通常表示（情報ログ）
            ImGui::TextUnformatted(formatted.c_str());
            break;
        }
    }

    //----------------------------------------------------
    // 5. 自動スクロール処理
    //----------------------------------------------------
    // 既に最下部にいる場合のみ追従スクロールする
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
#endif // USE_IMGUI
}