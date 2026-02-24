#pragma once
#include <IEditorWindow.h>
#include <ImGuiManager.h>

class InspectorWindow : public IEditorWindow {
public:
    const char* GetName() const override { return "Inspector"; }
    void Draw() override {
        // ここに具体的なUIを書く
        ImGui::Text("Object Parameters");
        static float val = 0.0f;
        ImGui::SliderFloat("Scale", &val, 0.0f, 1.0f);
    }
};