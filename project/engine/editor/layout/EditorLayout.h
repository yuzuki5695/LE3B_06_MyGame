#pragma once
#include <vector>
#include <memory>
#include <IEditorWindow.h>
#include <SrvManager.h>

class EditorLayout {
public:
    EditorLayout() = default;
    ~EditorLayout() = default;

    // エディタ全体のレイアウトを描画するメイン関数
    void Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows);

    // レイアウトのリセット要求
    void RequestReset() { requestResetLayout_ = true; }

private:
    // 枠組みを構成する内部関数（旧Managerから移動）
    void ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows);
    void DrawLeftPanel(SrvManager* srvmanager);
    void DrawRightPanel(std::vector<std::unique_ptr<IEditorWindow>>& windows);

private:
    bool requestResetLayout_ = false;
};