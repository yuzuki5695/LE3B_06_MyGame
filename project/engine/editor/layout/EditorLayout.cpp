#include "EditorLayout.h"
#include <MessageService.h>
#include <CopylmageCommon.h>
#include <ImGuiManager.h>
#include <LogManager.h>
#include <externals/imgui/imgui_internal.h>
#include <EditorManager.h>

using namespace Editor;

/// <summary>
/// 多言語テキスト取得用の短縮ラムダ
/// </summary>
std::function<std::string(const std::string&)> LT = [](const std::string& key) { return MessageService::GetText(key); };

void EditorLayout::Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    // 1. メインメニューバー
    ShowMenuBar(windows);
}

void EditorLayout::ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    if (ImGui::BeginMainMenuBar()) {
        // 表示メニュー
        if (ImGui::BeginMenu(LT("Menu.Settings").c_str())) {
            // 「言語設定」サブメニュー
            if (ImGui::BeginMenu(LT("Language.Title").c_str())) {
                // 日本語への切り替え
                bool isJp = (MessageService::GetCurrentLanguage() == Language::Japanese);
                if (ImGui::MenuItem(LT("Language.Japanese").c_str(), nullptr, isJp)) {
                    MessageService::SetLanguage(Language::Japanese);
                    // 必要ならここにログ出力などを追加
                }
                // 英語への切り替え
                bool isEn = (MessageService::GetCurrentLanguage() == Language::English);
                if (ImGui::MenuItem(LT("Language.English").c_str(), nullptr, isEn)) {
                    MessageService::SetLanguage(Language::English);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        // ... 他のメニュー項目（Optionsなど） ...
        ImGui::EndMainMenuBar();
    }
}