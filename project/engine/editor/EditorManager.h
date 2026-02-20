#pragma once
#include <vector>
#include <memory>
#include <IEditorWindow.h>
#include<SrvManager.h>

class EditorManager {
public:
    static EditorManager* GetInstance();

    // ウィンドウの登録（例：InspectorWindowを追加する等）
    void AddWindow(std::unique_ptr<IEditorWindow> window);

    // 全体の更新と描画
    void Draw(SrvManager* srvmanager);

private:
    EditorManager() = default;
    static EditorManager* instance;
    std::vector<std::unique_ptr<IEditorWindow>> windows_;

    // 内部的なメニューバーの描画
    void ShowMenuBar();
};