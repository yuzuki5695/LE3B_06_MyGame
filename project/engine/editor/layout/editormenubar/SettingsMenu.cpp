#include "SettingsMenu.h"
#include <MessageService.h>
#include <EditorConsole.h>
#include <ImGuiManager.h>

using namespace Editor;

void SettingsMenu::Render(const std::function<std::string(const std::string&)>& LT) {
#ifdef USE_IMGUI
    // 「Settings」メニュー開始
    // 開いていない場合は即終了
    if (!ImGui::BeginMenu(LT("Menu.Settings").c_str()))
        return;
    // -----------------------------
    // 言語設定サブメニュー
    // -----------------------------
    if (ImGui::BeginMenu(LT("Language.Title").c_str())) {
        // 現在の言語が日本語かどうか
        bool isJp = (MessageService::GetCurrentLanguage() == Language::Japanese);

        // 日本語選択
        if (ImGui::MenuItem(LT("Language.Japanese").c_str(), nullptr, isJp)) {
            // 言語変更
            MessageService::SetLanguage(Language::Japanese);
            // ログ出力
            EditorConsole::GetInstance()->AddLocalizedLog("System.LangChanged");
        }

        // 現在の言語が英語かどうか
        bool isEn = (MessageService::GetCurrentLanguage() == Language::English);

        // 英語選択
        if (ImGui::MenuItem(LT("Language.English").c_str(), nullptr, isEn)) {
            // 言語変更
            MessageService::SetLanguage(Language::English);
            // ログ出力
            EditorConsole::GetInstance()->AddLocalizedLog("System.LangChanged");
        }

        // Languageサブメニュー終了
        ImGui::EndMenu();
    }
    // Settingsメニュー終了
    ImGui::EndMenu();
#endif // USE_IMGUI
}