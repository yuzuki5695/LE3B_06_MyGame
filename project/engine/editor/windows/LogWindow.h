#pragma once
#include "IEditorWindow.h"
#include <string>
#include <vector>

class LogWindow : public IEditorWindow {
public:
    LogWindow();
    ~LogWindow() override = default;

    // IEditorWindow のインターフェース実装
    void Draw() override;
    const char* GetName() const override { return "System Console"; }

private:
};