#include "SettingsMenu.h"
#include <MessageService.h>
#include <EditorConsole.h>
#include <ImGuiManager.h>
#include <EditorLayout.h>

namespace MyEngine {

    using namespace EditorTypes;

    void SettingsMenu::Render(const std::function<std::string(const std::string&)>& LT) {
#ifdef USE_IMGUI
        // 「Settings」メニュー開始
        // 開いていない場合は即終了
        if (!ImGui::BeginMenu(LT("Menu.Settings").c_str()))
            return;

        // ====================================================
        // 全表示
        // ====================================================
        bool fullscreen = layout_ && layout_->IsGameViewFullscreen();
        if (ImGui::MenuItem(LT("Fullscreen").c_str(), "", fullscreen)) {
            layout_->ToggleGameViewFullscreen();
        }
        ImGui::Separator();

        // Settingsメニュー終了
        ImGui::EndMenu();
#endif // USE_IMGUI
    }
}