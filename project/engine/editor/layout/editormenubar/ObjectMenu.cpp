#include "ObjectMenu.h"
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI

namespace MyEngine {
    void ObjectMenu::Render(const std::function<std::string(const std::string&)>& LT) {
#ifdef USE_IMGUI
        // 「オブジェクト」メニュー開始
        if (!ImGui::BeginMenu(LT("Menu.Object").c_str())) {
            return;
        };

        // -------------------------
        // 3Dオブジェクトカテゴリ
        // -------------------------
        DrawCategoryMenu(
            LT("Object.Category3D").c_str(),
            Editor::EditorObjectCategory::Object3D,
            EditorEntityRegistry::Instance().GetObjects()
        );

        // -------------------------
        //  2Dオブジェクトカテゴリ
        // -------------------------
        DrawCategoryMenu(
            LT("Object.Category2D").c_str(),
            Editor::EditorObjectCategory::Object2D,
            EditorEntityRegistry::Instance().GetObjects()
        );

        ImGui::EndMenu();
#endif // USE_IMGUI
    }

    void ObjectMenu::DrawCategoryMenu(const char* menuName, Editor::EditorObjectCategory category, const std::vector<Editor::EditorObjectInfo>& objects) {
#ifdef USE_IMGUI 
        // カテゴリメニュー開始
        if (!ImGui::BeginMenu(menuName)) {
            return;
        }

        // 指定カテゴリのオブジェクトのみ表示
        for (const Editor::EditorObjectInfo& obj : objects) {

            // カテゴリが異なる場合はスキップ
            if (obj.category != category) {
                continue;
            }
            // オブジェクト選択
            if (ImGui::MenuItem(obj.name.c_str())) {
                // ウィンドウを開く
                OpenWindow(obj);
            }
        }

        ImGui::EndMenu();
#endif // USE_IMGUI
    }

    void ObjectMenu::OpenWindow(const Editor::EditorObjectInfo& obj) {
#ifdef USE_IMGUI 
        // -------------------------
        // 既に開いているか確認
        // -------------------------

        std::vector<Editor::EditorObjectInfo>::iterator it = std::find_if(
            openWindows_.begin(),
            openWindows_.end(),
            [&](const Editor::EditorObjectInfo& item) {
                // 同一ポインタなら既に開いている
                return item.objectPtr == obj.objectPtr;
            });
        // -------------------------
        // 未登録なら追加
        // -------------------------
        if (it == openWindows_.end()) {
            openWindows_.push_back(obj);
        }
#endif // USE_IMGUI
    }

    std::vector<Editor::EditorObjectInfo>::iterator ObjectMenu::CloseWindow(const std::string& name) {
#ifdef USE_IMGUI 
        std::vector<Editor::EditorObjectInfo>::iterator it = std::find_if(openWindows_.begin(), openWindows_.end(),
            [&](const Editor::EditorObjectInfo& obj) { return obj.name == name; });

        if (it != openWindows_.end()) {
            return openWindows_.erase(it); // リストから削除して次の要素を返す
        }
        return openWindows_.end();
#endif // USE_IMGUI
    }
}