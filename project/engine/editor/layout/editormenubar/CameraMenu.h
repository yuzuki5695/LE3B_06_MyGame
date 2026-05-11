#pragma once
#include <IMenuComponent.h>
#include <EditorTypes.h>

namespace MyEngine {
    /// <summary>
    /// 「設定」メニューを担当するクラス。
    /// </summary>
    class CameraMenu : public IMenuComponent {
    public: // メンバ関数
        /// <summary>
        /// 設定メニューの描画
        /// </summary>
        /// <param name="LT">多言語テキスト取得ラムダ</param>
        void Render(const std::function<std::string(const std::string&)>& LT) override;        
        /// <summary>
        /// オブジェクトウィンドウを開く
        /// 既に開いている場合は追加しない
        /// </summary>
        void OpenWindow(const EditorTypes::EditorObjectInfo& obj);
        /// <summary>
        /// 指定された名前のウィンドウを閉じる
        /// </summary>
        std::vector<EditorTypes::EditorObjectInfo>::iterator CloseWindow(const std::string& name);
        /// <summary>
        /// 開いているオブジェクトウィンドウを全て閉じる
        /// </summary>
        void ClearOpenWindows();
    private :
        std::vector<EditorTypes::EditorObjectInfo> openWindows_;
    public:
        std::vector<EditorTypes::EditorObjectInfo>& GetOpenWindows() { return openWindows_; }
    };
}