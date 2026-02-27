#pragma once
#include <vector>
#include <type_traits>
#include <EditorTypes.h>
#include <Object3d.h>
#include <Sprite.h>

/// <summary>
/// 型チェック用の常にfalseなテンプレート（static_assert用）
/// </summary>
template <typename T>
inline constexpr bool always_false = false;

/// <summary>
/// エディタ用オブジェクト登録管理クラス
/// ・アプリケーション層から登録されたオブジェクト情報を保持
/// </summary>
class EditorObjectRegistry {
public: // メンバ関数
    /// <summary>
    /// シングルトン取得
    /// エディタ全体で共通の登録管理を行うため
    /// </summary>
    static EditorObjectRegistry& Instance() {
        static EditorObjectRegistry instance;
        return instance;
    }

    /// <summary>
    /// オブジェクト情報を登録する
    /// アプリ層のコンストラクタや初期化処理から呼び出す想定
    /// </summary>
    void Register(const Editor::EditorObjectInfo& info) { objects_.push_back(info); }

    /// <summary>
    /// 登録済みオブジェクト一覧を取得
    /// 主にObjectMenu描画時に使用
    /// </summary>
    const std::vector<Editor::EditorObjectInfo>& GetObjects() const { return objects_; }

private: // メンバ変数
    // 登録されたオブジェクト情報の配列
    std::vector<Editor::EditorObjectInfo> objects_;
    // 外部から直接生成されないようにする（シングルトン）
    EditorObjectRegistry() = default;
};

/// <summary>
/// Object3d / Sprite などのオブジェクトをエディタ用に自動登録するヘルパー関数
/// ・生成した直後に呼ぶ
/// ・型によって自動でカテゴリを判定し、EditorObjectRegistry に登録
/// </summary>
template <typename T>
void RegisterEditorObject(T* object, const std::string& name) {
    // 登録カテゴリを型に応じて自動判定
    Editor::EditorObjectCategory category;

    if constexpr (std::is_same_v<T, Object3d>) {
        category = Editor::EditorObjectCategory::Object3D;
    } else if constexpr (std::is_same_v<T, Sprite>) {
        category = Editor::EditorObjectCategory::Object2D;
    } else {
        static_assert(always_false<T>, "Unsupported type for EditorObjectRegistry");
    }

    // EditorObjectInfo を作る
    Editor::EditorObjectInfo info;
    info.name = name;
    info.category = category;
    info.objectPtr = object;

    EditorObjectRegistry::Instance().Register(info);
}