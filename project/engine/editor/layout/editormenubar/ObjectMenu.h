#pragma once
#include <IMenuComponent.h>
#include <EditorEntityRegistry.h>

namespace MyEngine {
    /// <summary>
    /// 「オブジェクト」メニューを担当するクラス。
    /// 登録済みのエディタオブジェクトをカテゴリごとに表示し
    /// 選択されたオブジェクトのウィンドウ管理を行う
    /// </summary>
    class ObjectMenu : public IMenuComponent {
    public: // メンバ関数
        /// <summary>
        /// オブジェクトメニュー描画
        /// </summary>
        void Render(const std::function<std::string(const std::string&)>& LT) override;
        /// <summary>
        /// 指定カテゴリのサブメニューを描画する
        /// 
        /// 例:
        /// - Object3D
        /// - Object2D
        /// </summary>
        void DrawCategoryMenu(const char* menuName, Editor::EditorObjectCategory category, const std::vector<Editor::EditorObjectInfo>& objects); 
        /// <summary>
        /// オブジェクトウィンドウを開く
        /// 
        /// 既に開いている場合は追加しない
        /// </summary>
        void OpenWindow(const Editor::EditorObjectInfo& obj);
        /// <summary>
        /// 指定された名前のウィンドウを閉じる
        /// </summary>
        std::vector<Editor::EditorObjectInfo>::iterator CloseWindow(const std::string& name);
    private: // メンバ変数
        std::string activeObjectName_;                        /// 現在選択中のオブジェクト名
        void* activeObject_ = nullptr;                        /// 現在選択中のオブジェクト
        std::vector<Editor::EditorObjectInfo> openWindows_;   /// 現在開いているオブジェクトウィンドウ一覧             
    public: // アクセッサ
        void* GetActiveObject() const { return activeObject_; }
        // 戻り値に & を付けて参照を返すようにする
        const std::vector<Editor::EditorObjectInfo>& GetOpenWindows() const { return openWindows_; }
        // 非const版（削除操作などでイテレータをいじる場合用）
        std::vector<Editor::EditorObjectInfo>& GetOpenWindows() { return openWindows_; }
    };
}