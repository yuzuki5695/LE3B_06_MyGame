#include "EditorMenuBar.h"
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI

namespace MyEngine {
    void EditorMenuBar::Render(const std::function<std::string(const std::string&)>& LT) {
#ifdef USE_IMGUI
        // メインメニューバー開始
        if (!ImGui::BeginMainMenuBar()) {
            return;
        }
        // 登録済みメニューを順番に描画
        for (const std::unique_ptr<IMenuComponent>& menu : menuComponents_) {
            menu->Render(LT);
        }
        // メインメニューバー終了
        ImGui::EndMainMenuBar();
#endif
    }
}