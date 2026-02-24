#include "EditorLayout.h"
#include <MessageService.h>
#include <CopylmageCommon.h>
#include <ImGuiManager.h>
#include <LogManager.h>
#include <externals/imgui/imgui_internal.h>
#include <EditorManager.h>

void EditorLayout::Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    // 1. メインメニューバー
    ShowMenuBar(windows);

    //// 2. 左側：ゲームビューなど（固定領域）
    //DrawLeftPanel(srvmanager);

    //// 3. 右側：ドッキングスペース（可変領域）
    //DrawRightPanel(windows);
}

void EditorLayout::ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    if (ImGui::BeginMainMenuBar()) {
        // 表示メニュー
        if (ImGui::BeginMenu(MessageService::GetText("Editor.Menu.View").c_str())) {
            for (auto& window : windows) {
                if (ImGui::MenuItem(window->GetName(), nullptr, window->isOpen)) {
                    window->isOpen = !window->isOpen;
                }
            }
            ImGui::EndMenu();
        }
        // ... 他のメニュー項目（Optionsなど） ...
        ImGui::EndMainMenuBar();
    }
}

void EditorLayout::DrawLeftPanel(SrvManager* srvmanager) {
    const float kGameViewW = 640.0f;
    const float kGameViewH = 360.0f;
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2(kGameViewW, viewport->WorkSize.y));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;

    ImGui::Begin("LeftPanel", nullptr, flags);
    {
        // --- 上段：ゲームビュー ---
        ImGui::BeginChild("GameView", ImVec2(kGameViewW, kGameViewH), true, ImGuiWindowFlags_NoDocking);
        uint32_t srvIdx = CopylmageCommon::GetInstance()->GetSrvIndex();
        ImTextureID texID = (ImTextureID)(void*)(uintptr_t)srvmanager->GetGPUDescriptorHandle(srvIdx).ptr;
        ImGui::Image(texID, ImVec2(kGameViewW, kGameViewH));
        ImGui::EndChild();

        // --- 下段：ログ表示（LogManagerから取得） ---
        ImGui::Text("--- System Console ---");
        if (ImGui::Button("Clear")) { LogManager::GetInstance()->Clear(); }

        ImGui::BeginChild("LogWindow", ImVec2(0, 0), true, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto& log : LogManager::GetInstance()->GetLogs()) {
            // 特定のキーワードが含まれていたら色を変える
            if (log.find("ERROR") != std::string::npos) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), log.c_str()); // 赤
            } else if (log.find("Warning") != std::string::npos) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), log.c_str()); // 黄
            } else if (log.find("[System]") != std::string::npos) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), log.c_str()); // 水色
            } else {
                ImGui::TextUnformatted(log.c_str()); // 通常
            }
        }
        // 新しいログが来たら自動スクロール
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void EditorLayout::DrawRightPanel(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
   const float kGameViewW = 640.0f;
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    float dockPosX = viewport->WorkPos.x + kGameViewW;
    float dockWidth = viewport->WorkSize.x - kGameViewW;

    ImGui::SetNextWindowPos(ImVec2(dockPosX, viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(dockWidth, viewport->WorkSize.y));

    ImGuiWindowFlags root_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    ImGui::Begin("RightRoot", nullptr, root_flags);
    ShowMenuBar(windows);

    ImGuiID dockspace_id = ImGui::GetID("RightDockSpace");

    // 初回起動時やリセット要求時にレイアウトを構築
    if (requestResetLayout_ || !ImGui::DockBuilderGetNode(dockspace_id)) {
        requestResetLayout_ = false;

        // 既存のレイアウトを破棄して作り直し
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        ImGuiID dock_main_id = dockspace_id;

        // 登録されているすべてのウィンドウをメインのドッキングスペースに流し込む
        for (auto& window : windows) {
            ImGui::DockBuilderDockWindow(window->GetName(), dock_main_id);
        }

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();
}