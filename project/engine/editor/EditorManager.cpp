#include "EditorManager.h"
#include<ImGuiManager.h>
#include <CopylmageCommon.h>

EditorManager* EditorManager::instance = nullptr;

EditorManager* EditorManager::GetInstance() {
    if (!instance) instance = new EditorManager();
    return instance;
}

void EditorManager::AddWindow(std::unique_ptr<IEditorWindow> window) {
    windows_.push_back(std::move(window));
}

void EditorManager::Draw(SrvManager* srvmanager) {
#ifdef USE_IMGUI
    const float kGameViewW = 640.0f;
    const float kGameViewH = 360.0f;
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // --- 1. 左側エリア (ドッキング禁止・固定枠) ---
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2(kGameViewW, viewport->WorkSize.y));
    
    // 【重要】ImGuiWindowFlags_NoDocking を追加
    ImGuiWindowFlags leftAreaFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
                                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;

    ImGui::Begin("LeftArea_Fixed", nullptr, leftAreaFlags);
    {
        // 上段：ゲームビュー
        ImGui::BeginChild("GameViewChild", ImVec2(kGameViewW, kGameViewH), true, ImGuiWindowFlags_NoDocking);
        {
            uint32_t srvIdx = CopylmageCommon::GetInstance()->GetSrvIndex();
            ImTextureID texID = (ImTextureID)(void*)(uintptr_t)srvmanager->GetGPUDescriptorHandle(srvIdx).ptr;
            ImGui::Image(texID, ImVec2(kGameViewW, kGameViewH));
        }
        ImGui::EndChild();

        // 中間の区切り
        ImGui::Separator();

        // 下段：ログ・コンソールエリア (ここもドッキング不可の固定表示)
        ImGui::Text("--- System Log ---");
        // 残りの高さをすべて使う child window
        ImGui::BeginChild("LogAreaFixed", ImVec2(0, 0), false, ImGuiWindowFlags_NoDocking);
        {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[11:00:01] Engine Initialized.");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[11:00:05] Warning: Texture 'stone.png' not found.");
            // 将来ここにログをループで回して表示
        }
        ImGui::EndChild();
    }
    ImGui::End();

    // --- 2. 右側エリア (こちらはドッキングを許可) ---
    float dockPosX = viewport->WorkPos.x + kGameViewW;
    float dockWidth = viewport->WorkSize.x - kGameViewW;

    ImGui::SetNextWindowPos(ImVec2(dockPosX, viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(dockWidth, viewport->WorkSize.y));

    // 右側はドッキングの土台なので NoDocking はつけない
    ImGuiWindowFlags rightRootFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | 
                                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    ImGui::Begin("RightRoot", nullptr, rightRootFlags);
    {
        ShowMenuBar();
        ImGuiID dockspace_id = ImGui::GetID("RightDockSpace");
        // ここにだけドッキングを許可する
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
    }
    ImGui::End();

    // --- 3. 登録された各ウィンドウの描画 (Inspectorなど) ---
    for (auto& window : windows_) {
        if (window->isOpen) {
            if (ImGui::Begin(window->GetName(), &window->isOpen)) {
                window->Draw();
            }
            ImGui::End();
        }
    }
#endif
}

void EditorManager::ShowMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("メニュー")) {
            for (auto& window : windows_) {
                // メニューをクリックしたらフラグをONにする
                if (ImGui::MenuItem(window->GetName())) {
                    window->isOpen = true;
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}