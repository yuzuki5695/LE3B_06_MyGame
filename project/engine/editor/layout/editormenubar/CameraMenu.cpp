#include "CameraMenu.h"
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <CameraManager.h>

namespace MyEngine {

    using namespace EditorTypes;

    void CameraMenu::Render(
        const std::function<std::string(const std::string&)>& LT) {

#ifdef USE_IMGUI

        CameraManager* cameraManager = CameraManager::GetInstance();

        if (!cameraManager) {
            return;
        }

        if (ImGui::MenuItem(
            LT("Menu.Camera").c_str())) {

            CameraEditorInfo info;
            info.name = "Camera";            
            info.cameraPtr = cameraManager;
            // 中身はCameraManagerに任せる
            info.drawEditor = [cameraManager]() {cameraManager->GetActiveCamera()->DrawImGui(); };
            OpenWindow(info);
        }

#endif // USE_IMGUI
    }

    void CameraMenu::OpenWindow(const CameraEditorInfo& info) {
#ifdef USE_IMGUI
        auto it = std::find_if(
            openWindows_.begin(),
            openWindows_.end(),
            [&](const CameraEditorInfo& item) {
                return item.cameraPtr == info.cameraPtr;
            });

        if (it == openWindows_.end()) {
            openWindows_.push_back(info);
        }
#endif
    }

    std::vector<CameraEditorInfo>::iterator CameraMenu::CloseWindow(const std::string& name) {
#ifdef USE_IMGUI

        auto it = std::find_if(
            openWindows_.begin(),
            openWindows_.end(),
            [&](const CameraEditorInfo& item) {
                return item.name == name;
            });

        if (it != openWindows_.end()) {
            return openWindows_.erase(it);
        }

        return openWindows_.end();

#endif
    }
        
    void CameraMenu::ClearOpenWindows() {
#ifdef USE_IMGUI 
        openWindows_.clear();
#endif // USE_IMGUI
    }
}