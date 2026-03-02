#include "ObjectMenu.h"
#include <ImGuiManager.h>

void ObjectMenu::Render(const std::function<std::string(const std::string&)>& LT) {
    // 「オブジェクト」メニュー開始
    if (!ImGui::BeginMenu(LT("Menu.Object").c_str()))
        return;

    // すべての登録オブジェクトを取得
    const auto& objects = EditorEntityRegistry::Instance().GetObjects();

    // -------------------------
    // 3D カテゴリ
    // -------------------------
    if (ImGui::BeginMenu(LT("Object.Category3D").c_str())) {
        for (const auto& obj : objects) {
            if (obj.category == Editor::EditorObjectCategory::Object3D) {
                if (ImGui::MenuItem(obj.name.c_str())) {
                    auto it = std::find_if(openWindows_.begin(), openWindows_.end(),
                        [&](const auto& item) { return item.objectPtr == obj.objectPtr; });
                    if (it == openWindows_.end()) {
                        openWindows_.push_back(obj);
                    }
                }
            }
        }
        ImGui::EndMenu();
    }

    // -------------------------
    // 2D カテゴリ
    // -------------------------
    if (ImGui::BeginMenu(LT("Object.Category2D").c_str())) {
        for (const auto& obj : objects) {
            if (obj.category == Editor::EditorObjectCategory::Object2D) {
                if (ImGui::MenuItem(obj.name.c_str())) {
                    // リストに重複がなければ追加
                    auto it = std::find_if(openWindows_.begin(), openWindows_.end(),
                        [&](const auto& item) { return item.objectPtr == obj.objectPtr; });
                    if (it == openWindows_.end()) {
                        openWindows_.push_back(obj);
                    }
                }
            }
        }
        ImGui::EndMenu();
    }
    ImGui::EndMenu();
}

std::vector<Editor::EditorObjectInfo>::iterator ObjectMenu::CloseWindow(const std::string& name) {
    auto it = std::find_if(openWindows_.begin(), openWindows_.end(), 
        [&](const auto& obj) { return obj.name == name; });
    
    if (it != openWindows_.end()) {
        return openWindows_.erase(it); // リストから削除して次の要素を返す
    }
    return openWindows_.end();
}