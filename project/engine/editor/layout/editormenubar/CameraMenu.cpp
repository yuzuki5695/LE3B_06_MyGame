#include "CameraMenu.h"
#include <CameraManager.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <EditorEntityRegistry.h>

namespace MyEngine {
        
    using namespace EditorTypes;

    void CameraMenu::Render(const std::function<std::string(const std::string&)>& LT) {
#ifdef USE_IMGUI
        //if (!ImGui::BeginMenu(LT("Menu.Camera").c_str())) {
        //    return;
        //}

        //const std::vector<EditorObjectInfo>& items = EditorEntityRegistry::Instance().GetObjects();

        //for (const auto& item : items) {

        //    if (item.category != ObjectCategory::Camera) {
        //        continue;
        //    }

        //    if (ImGui::MenuItem(item.name.c_str())) {
        //        OpenWindow(item);
        //    }
        //}

        //ImGui::EndMenu();
#endif // USE_IMGUI
    }

    void CameraMenu::OpenWindow(const EditorObjectInfo& obj) {
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

    std::vector<EditorObjectInfo>::iterator CameraMenu::CloseWindow(const std::string& name) {
#ifdef USE_IMGUI 
        std::vector<EditorObjectInfo>::iterator it = std::find_if(openWindows_.begin(), openWindows_.end(),
            [&](const EditorObjectInfo& obj) { return obj.name == name; });

        if (it != openWindows_.end()) {
            return openWindows_.erase(it); // リストから削除して次の要素を返す
        }
        return openWindows_.end();
#endif // USE_IMGUI
    }
    void CameraMenu::ClearOpenWindows() {
#ifdef USE_IMGUI 
        openWindows_.clear();
#endif // USE_IMGUI
    }
}