#include "EditorMenuBar.h"
#include <ImGuiManager.h>
#include <MessageService.h>
#include <EditorConsole.h>

namespace Editor {
    void MenuBar::Render(const std::function<std::string(const std::string&)>& LT) {
        // メインメニューバー開始 
        if (!ImGui::BeginMainMenuBar())
            return;

        // ------------------------------------
        // 各メニューの描画
        // ------------------------------------

        settingsMenu_.Render(LT);  // 「設定」メニューの描画
		objectMenu_.Render(LT);    // 「オブジェクト」メニューの描画

        // メインメニューバー終了
        ImGui::EndMainMenuBar();
    }
}