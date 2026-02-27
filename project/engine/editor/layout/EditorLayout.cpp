#include "EditorLayout.h"
#include <MessageService.h>
#include <CopylmageCommon.h>
#include <ImGuiManager.h>
#include <EditorConsole.h>
#include <externals/imgui/imgui_internal.h>
#include <EditorManager.h>

using namespace Editor;

void EditorLayout::Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    LT = [](const std::string& key) { return MessageService::GetText(key); };
    // 1. メインメニュ-パネル
    ShowMenuBar(windows);
    // 2. 左側パネル
    DrawLeftPanel(srvmanager,windows);
    // 2. 右側パネル
	DrawRightPanel(windows);
}

void EditorLayout::ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    // UI描画はMenuBarへ委譲
    menuBar_.Render(LT);
}

void EditorLayout::DrawLeftPanel(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    // パネルの基本サイズ設定
    const float kGameViewW = 640.0f;
    const float kGameViewH = 360.0f;
    // メイン画面の表示領域を取得
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    // パネルの位置とサイズを固定（左端に配置）
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2(kGameViewW, viewport->WorkSize.y));
    // パネルの挙動フラグ（移動不可、リサイズ不可、タイトルバーなし、ドッキング不可）
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;

    ImGui::Begin("LeftPanel", nullptr, flags);
    {
        // 上段：ゲームビュー(描画結果をテクスチャとして表示）
        ImGui::BeginChild("GameView", ImVec2(kGameViewW, kGameViewH), true, ImGuiWindowFlags_NoDocking);
        {
            // レンダーターゲットからSRVインデックスを取得し、ImGui用のテクスチャIDに変換
            uint32_t srvIdx = CopylmageCommon::GetInstance()->GetSrvIndex();
            ImTextureID texID = (ImTextureID)(void*)(uintptr_t)srvmanager->GetGPUDescriptorHandle(srvIdx).ptr;

            // 指定サイズで画像を描画
            ImGui::Image(texID, ImVec2(kGameViewW, kGameViewH));
        }
        ImGui::EndChild();

        // 下段：ログ表示
        ImGui::BeginChild("SystemConsoleChild", ImVec2(0, 0), false);
        {
            // windowsリストの中から ConsoleWindow (System Console) を探して描画する
            for (auto& window : windows) {
                if (std::string(window->GetName()) == "System Console") {
                    ImGui::Text(LT("Console.Title").c_str());
                    ImGui::Separator();
                    window->Draw();
                    break;
                }
            }
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

    ImGuiWindowFlags root_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    // ドッキングスペース
    static bool dock_initialized = false;
    if (!dock_initialized) {
        ImGuiID dockspace_id = ImGui::GetID("RightDockSpace");
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        ImGui::DockBuilderDockWindow("Inspector", dockspace_id);

        ImGui::DockBuilderFinish(dockspace_id);
        dock_initialized = true;
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }

    // DrawRightPanel 内での描画
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoCollapse);
    auto [obj, category] = menuBar_.GetActiveObject(); // ObjectMenu 経由で取得
    if (obj) {
        switch (category) {
        case EditorObjectCategory::Object3D:
            static_cast<Object3d*>(obj)->DrawImGui("Inspector");
            break;
        case EditorObjectCategory::Object2D:
           // static_cast<Sprite*>(obj)->DrawImGui("Inspector");
            break;
        default:
            ImGui::Text("Unknown object type");
            break;
        }
    } else {
        ImGui::Text("No Selection");
    }

    ImGui::End();
}