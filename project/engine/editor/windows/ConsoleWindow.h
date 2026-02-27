#pragma once
#include "IEditorWindow.h"
#include <string>
#include <vector>

class ConsoleWindow : public IEditorWindow {
public:
    ConsoleWindow();
    ~ConsoleWindow() override = default;

    // IEditorWindow のインターフェース実装
    void Draw() override;
    const char* GetName() const override { return "System Console"; }

private:
};