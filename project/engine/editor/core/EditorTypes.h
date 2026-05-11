#pragma once
#include <string>
#include <functional>

namespace MyEngine {
    namespace Editor {
        // 言語設定の列挙体
        enum class Language {
            Japanese, // 日本語
            English   // 英語
            // 将来的に追加可能
        };

        /// エディタ上で扱うオブジェクトのカテゴリ分類
        enum class EditorObjectCategory {
            Object3D,  // 3Dオブジェクト
            Object2D,   // 2Dオブジェクト
            Player,
        };

        /// エディタに登録されるオブジェクトの基本情報
        struct EditorObjectInfo {
            std::string name;                        // エディタ表示用の名前    
            Editor::EditorObjectCategory category;   // 分類カテゴリ
            void* objectPtr = nullptr;               // 任意のオブジェクトポインタを保持(Object3dまたはSprite*)
            // エディタ描画関数
            std::function<void(void*)> drawFunc;
            // 生存確認
            std::function<bool(void*)> aliveFunc;
        };
    }
}