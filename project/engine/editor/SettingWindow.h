#pragma once
#include "IEditorWindow.h"
#include <MessageService.h>

class SettingWindow : public IEditorWindow {
public:
    SettingWindow();
    ~SettingWindow() override = default;

    void Draw() override;
    void OnMenuBar() override;
    const char* GetName() const override { return "Settings"; }

private:
    // 言語を切り替える内部処理
    void SetLanguage(Editor::Language lang);
};