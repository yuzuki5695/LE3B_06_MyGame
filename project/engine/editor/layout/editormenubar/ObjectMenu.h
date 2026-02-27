#pragma once
#include <IMenuComponent.h>
#include <EditorObjectRegistry.h>

/// <summary>
/// 「オブジェクト」メニューを担当するクラス。
/// </summary>
class ObjectMenu : public IMenuComponent {
public: // メンバ関数
    /// <summary>
    /// オブジェクトメニュー描画
    /// </summary>
    void Render(const std::function<std::string(const std::string&)>& LT) override;
private: // メンバ変数
    std::string activeObjectName_;
    void* activeObject_ = nullptr;
    Editor::EditorObjectCategory activeCategory_;
public: // アクセッサ
    void* GetActiveObject() const { return activeObject_; }
    Editor::EditorObjectCategory GetActiveCategory() const { return activeCategory_; }
};