#include "ObjectMenu.h"
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <EditorEntityRegistry.h>

namespace MyEngine {
    
    using namespace EditorTypes;

    void ObjectMenu::Render(const std::function<std::string(const std::string&)>& LT) {
#ifdef USE_IMGUI
        // 「オブジェクト」メニュー開始
        if (!ImGui::BeginMenu(LT("Menu.Object").c_str())) {
            return;
        }

        const std::vector<EditorObjectInfo>& objects = EditorEntityRegistry::Instance().GetObjects();

        for (const CategoryInfo& info : kCategoryInfos) {
            DrawCategoryMenu(LT(info.translationKey).c_str(), info.category, objects);
        }

        ImGui::EndMenu();
#endif // USE_IMGUI
    }

    void ObjectMenu::DrawCategoryMenu(const char* menuName, ObjectCategory  category, const std::vector<EditorObjectInfo>& objects) {
#ifdef USE_IMGUI 
        // カテゴリメニュー開始
        if (!ImGui::BeginMenu(menuName)) {
            return;
        }

        // 指定カテゴリのオブジェクトのみ表示
        for (const EditorObjectInfo& obj : objects) {

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

    void ObjectMenu::OpenWindow(const EditorObjectInfo& obj) {
#ifdef USE_IMGUI 
        // -------------------------
        // 既に開いているか確認
        // -------------------------

        std::vector<EditorObjectInfo>::iterator it = std::find_if(
            openWindows_.begin(),
            openWindows_.end(),
            [&](const EditorObjectInfo& item) {
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

    std::vector<EditorObjectInfo>::iterator ObjectMenu::CloseWindow(const std::string& name) {
#ifdef USE_IMGUI 
        std::vector<EditorObjectInfo>::iterator it = std::find_if(openWindows_.begin(), openWindows_.end(),
            [&](const EditorObjectInfo& obj) { return obj.name == name; });

        if (it != openWindows_.end()) {
            return openWindows_.erase(it); // リストから削除して次の要素を返す
        }
        return openWindows_.end();
#endif // USE_IMGUI
    }
    
    void ObjectMenu::ClearOpenWindows() {
#ifdef USE_IMGUI 
        openWindows_.clear();
#endif // USE_IMGUI
    }
}