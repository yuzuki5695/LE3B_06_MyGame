#include "EditorLayout.h"
#include <MessageService.h>
#include <CopylmageCommon.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <EditorConsole.h>
#include <externals/imgui/imgui_internal.h>
#include <EditorManager.h>
#include <SettingsMenu.h>
#include <ObjectMenu.h>
#include <CameraMenu.h>
#include <SceneMenu.h>
#include <SceneManager.h>

namespace MyEngine {

    using namespace EditorTypes;

    void EditorLayout::Initialize() {
#ifdef USE_IMGUI
		// メニューバーの初期化とメニューコンポーネントの追加
        menuBar_ = std::make_unique<EditorMenuBar>();
        menuBar_->AddMenu<SettingsMenu>()->SetLayout(this);
        menuBar_->AddMenu<SceneMenu>();
        menuBar_->AddMenu<ObjectMenu>();
        menuBar_->AddMenu<CameraMenu>();
#endif // USE_IMGUI
    }

    void EditorLayout::Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
#ifdef USE_IMGUI
        // 多言語テキスト取得ラムダを初期化
        LT = [](const std::string& key) { return MessageService::GetText(key); };
        // =========================================
        // GameViewフルスクリーン
        // =========================================
        if (isGameViewFullscreen_) {
            DrawFullscreenGameView(srvmanager);
            return;
        }

        // =========================================        
        // 通常エディタ
        // =========================================

        // 1. メインメニューバー描画
        ShowMenuBar(windows);
        // 2. 左側パネル描画（ゲームビュー + コンソール）
        DrawLeftPanel(srvmanager, windows);
        // 3. 右側パネル描画（ドッキング領域）
        DrawRightPanel(windows);
#endif // USE_IMGUI
    }

    void EditorLayout::ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
#ifdef USE_IMGUI
        // UI描画はMenuBarクラスへ委譲
        menuBar_->Render(LT);
#endif // USE_IMGUI
    }

    void EditorLayout::DrawLeftPanel(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
#ifdef USE_IMGUI
        // メイン画面の表示領域を取得
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        // パネルの位置とサイズを固定（左端に配置）
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(kGameViewWidth_, viewport->WorkSize.y));
        // パネルの挙動フラグ（移動不可、リサイズ不可、タイトルバーなし、ドッキング不可）
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;

        ImGui::Begin("LeftPanel", nullptr, flags); {
            // 上段：ゲームビュー(描画結果をテクスチャとして表示）
            ImGui::BeginChild("GameView", ImVec2(kGameViewWidth_, kGameViewHeight_), true, ImGuiWindowFlags_NoDocking); {
                // レンダーターゲットからSRVインデックスを取得し、ImGui用のテクスチャIDに変換
                uint32_t srvIdx = CopylmageCommon::GetInstance()->GetSrvIndex();
                ImTextureID texID = (ImTextureID)(void*)(uintptr_t)srvmanager->GetGPUDescriptorHandle(srvIdx).ptr;

                // 指定サイズで画像を描画
                ImGui::Image(texID, ImVec2(kGameViewWidth_, kGameViewHeight_));
            }
            ImGui::EndChild();

            // 下段：ログ表示
            ImGui::BeginChild("SystemConsoleChild", ImVec2(0, 0), false); {
                // windowsリストの中から ConsoleWindow (System Console) を探して描画する
                for (std::unique_ptr<IEditorWindow>& window : windows) {
                    if (std::string(window->GetName()) == "System Console") {
                        ImGui::Text(LT("Console.Title").c_str());
                        ImGui::Separator();
                        // 現在シーン名
                        SceneManager* sceneManager = SceneManager::GetInstance();
                        if (sceneManager->GetCurrentScene()) {
                            std::string currentScene = sceneManager->GetCurrentSceneName();
                            std::string localizedScene = LT("Scene." + currentScene);
                            ImGui::Text("%s : %s", LT("Scene.Current").c_str(), localizedScene.c_str());
                        }

                        ImGui::Separator();
                        window->Draw();
                        break;
                    }
                }
            }
            ImGui::EndChild();

        }
        ImGui::End();
#endif // USE_IMGUI
    }

    void EditorLayout::DrawFullscreenGameView(SrvManager* srvmanager) {
#ifdef USE_IMGUI
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDocking;

        ImGui::Begin("FullscreenGameView", nullptr, flags);

        uint32_t srvIdx = CopylmageCommon::GetInstance()->GetSrvIndex();
        ImTextureID texID = (ImTextureID)(void*)(uintptr_t)srvmanager->GetGPUDescriptorHandle(srvIdx).ptr;

        // 利用可能領域取得
        ImVec2 avail = ImGui::GetContentRegionAvail();

        // GameView描画
        ImGui::Image(texID, avail);
        ImGui::End();
#endif
    }

    void EditorLayout::DrawRightPanel(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
#ifdef USE_IMGUI
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        // 右側領域計算
        float dockPosX = viewport->WorkPos.x + kRightPanelViewWidth_;
        float dockWidth = viewport->WorkSize.x - kRightPanelViewWidth_;

        ImGui::SetNextWindowPos(ImVec2(dockPosX, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(dockWidth, viewport->WorkSize.y));
        // ルートウィンドウフラグ
        ImGuiWindowFlags root_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        // 右側ルートウィンドウ
        ImGui::Begin("RightPanelRoot", nullptr, root_flags);
        ImGuiID dockspace_id = ImGui::GetID("RightDockSpace");
        // 初回のみDock初期化
        static bool dock_initialized = false;
        if (!dock_initialized) {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderFinish(dockspace_id);
            dock_initialized = true;
        }

        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        ImGui::End();

        // =====================================
        // Object Window
        // =====================================
        const std::vector<EditorTypes::EditorObjectInfo>& registeredObjects = EditorEntityRegistry::Instance().GetObjects();
        auto& openWindows = menuBar_->GetMenu<ObjectMenu>()->GetOpenWindows();
        for (auto it = openWindows.begin();
            it != openWindows.end(); ) {
            // 生存チェック
            auto isAlive = std::any_of(
                registeredObjects.begin(),
                registeredObjects.end(),
                [&](const auto& reg) {
                    return reg.objectPtr == it->objectPtr;
                });

            if (!isAlive) {
                it = menuBar_->GetMenu<ObjectMenu>()->CloseWindow(it->name);
                continue;
            }

            bool is_open = BeginDockedWindow(*it, dockspace_id);

            if (!is_open) {
                it = menuBar_->GetMenu<ObjectMenu>()->CloseWindow(it->name);
            } else {
                ++it;
            }
        }

        // =====================================
        // Camera Window
        // =====================================
        std::vector<EditorTypes::CameraEditorInfo>& cameraWindows = menuBar_->GetMenu<CameraMenu>()->GetOpenWindows();
        for (auto it = cameraWindows.begin();
            it != cameraWindows.end(); ) {

            bool is_open =
                BeginDockedWindow(*it, dockspace_id);

            if (!is_open) {
                it = menuBar_->GetMenu<CameraMenu>()->CloseWindow(it->name);
            } else {
                ++it;
            }
        }

#endif // USE_IMGUI
    }
}