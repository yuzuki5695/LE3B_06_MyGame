#include "ObjectMenu.h"
#include <ImGuiManager.h>

void ObjectMenu::Render(const std::function<std::string(const std::string&)>& LT) {
    // 「オブジェクト」メニュー開始
    if (!ImGui::BeginMenu(LT("Menu.Object").c_str()))
        return;

    // すべての登録オブジェクトを取得
    const auto& objects = EditorObjectRegistry::Instance().GetObjects();

    // -------------------------
    // 3D カテゴリ
    // -------------------------
    if (ImGui::BeginMenu(LT("Object.Category3D").c_str())) {
        for (const auto& obj : objects) {
            if (obj.category == Editor::EditorObjectCategory::Object3D) {
                if (ImGui::MenuItem(obj.name.c_str())) {
                    // 選択中オブジェクト名を保存
                    activeObjectName_ = obj.name;
                    activeObject_ = obj.objectPtr;       // Object3d* を保存
                    activeCategory_ = obj.category;
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
                    //// 選択中オブジェクト名を保存
                    //activeObjectName_ = obj.name;
                    //activeObject_ = obj.objectPtr;       // Sprite* を保存
                    //activeCategory_ = obj.category;
                }
            }
        }
        ImGui::EndMenu();
    }

    ImGui::EndMenu();
}