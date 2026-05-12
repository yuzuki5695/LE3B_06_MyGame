#pragma once
#include <IMenuComponent.h>
#include <EditorEntityRegistry.h>

namespace MyEngine {
    /// <summary>
    /// 
    /// </summary>
    class CameraMenu : public IMenuComponent {
    public: // メンバ関数

        void Render(const std::function<std::string(const std::string&)>& LT) override;
        
        void OpenWindow(const EditorTypes::CameraEditorInfo& info);
        
        std::vector<EditorTypes::CameraEditorInfo>::iterator CloseWindow(const std::string& name);

        /// <summary>
        /// 開いているオブジェクトウィンドウを全て閉じる
        /// </summary>
        void ClearOpenWindows();
    private: // メンバ変数
        std::vector<EditorTypes::CameraEditorInfo> openWindows_;   /// 現在開いているオブジェクトウィンドウ一覧     
    public:
        std::vector<EditorTypes::CameraEditorInfo>& GetOpenWindows() { return openWindows_; }
    };
}