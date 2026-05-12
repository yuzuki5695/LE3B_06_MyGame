#pragma once
#include <string>
#include <functional>
#include <array>

namespace MyEngine {
    namespace EditorTypes {
        /// 言語設定の列挙体
        enum class Language {
            Japanese, // 日本語
            English   // 英語
            // 将来的に追加可能
        };

        /// エディタオブジェクトカテゴリ
        enum class ObjectCategory {
            Object3D,  // 3Dオブジェクト
            Object2D,   // 2Dオブジェクト
            Count
        };

        /// カテゴリ情報 
        struct CategoryInfo {
            ObjectCategory category;
            const char* translationKey;
        };

        /// カテゴリ一覧
        inline constexpr std::array<CategoryInfo,
            static_cast<size_t>(ObjectCategory::Count)> kCategoryInfos = { {
                { ObjectCategory::Object3D, "Object.Category3D" },
                { ObjectCategory::Object2D, "Object.Category2D" }
            }
        };

        /// エディタに登録されるオブジェクトの基本情報
        struct EditorObjectInfo {
            std::string name;                        // エディタ表示用の名前    
            ObjectCategory category;
            // 任意のオブジェクトポインタを保持
            void* objectPtr = nullptr;
            std::function<void()> drawEditor;
        };
        struct CameraEditorInfo {
            std::string name;
            void* cameraPtr = nullptr;
            std::function<void()> drawEditor;
        };
    }
}