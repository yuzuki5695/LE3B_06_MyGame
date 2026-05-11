#pragma once
#include <vector>
#include <type_traits>
#include <EditorTypes.h>
#include <Object3d.h>
#include <Sprite.h>

namespace MyEngine {
    /// <summary>
    /// 型チェック用の常にfalseなテンプレート（static_assert用）
    /// </summary>
    template <typename T>
    inline constexpr bool always_false = false;

    /// <summary>
    /// エディタ用オブジェクト登録管理クラス
    /// ・アプリケーション層から登録されたオブジェクト情報を保持
    /// </summary>
    class EditorEntityRegistry {
    public: // メンバ関数
        /// <summary>
        /// シングルトン取得
        /// エディタ全体で共通の登録管理を行うため
        /// </summary>
        static EditorEntityRegistry& Instance() {
            static EditorEntityRegistry instance;
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
    };
}