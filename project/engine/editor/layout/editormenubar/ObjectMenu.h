#pragma once
#include <IMenuComponent.h>
#include <EditorEntityRegistry.h>

/// <summary>
/// 「オブジェクト」メニューを担当するクラス。
/// </summary>
class ObjectMenu : public IMenuComponent {
public: // メンバ関数
    /// <summary>
    /// オブジェクトメニュー描画
    /// </summary>
    void Render(const std::function<std::string(const std::string&)>& LT) override;

    std::vector<Editor::EditorObjectInfo>::iterator CloseWindow(const std::string& name);

private: // メンバ変数
    std::string activeObjectName_;
    void* activeObject_ = nullptr;
    std::vector<Editor::EditorObjectInfo> openWindows_;
public: // アクセッサ
    void* GetActiveObject() const { return activeObject_; } 
    // 戻り値に & を付けて参照を返すようにする
    const std::vector<Editor::EditorObjectInfo>& GetOpenWindows() const { return openWindows_; }    
    // 非const版（削除操作などでイテレータをいじる場合用）
    std::vector<Editor::EditorObjectInfo>& GetOpenWindows() { return openWindows_; }
};