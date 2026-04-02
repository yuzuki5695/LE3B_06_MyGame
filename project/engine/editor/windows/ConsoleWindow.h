#pragma once
#include <IEditorWindow.h>
#include <string>
#include <vector>

namespace MyEngine {
    /// <summary>
    /// ImGuiエディタ内に表示される「システムコンソール」ウィンドウ表示専用クラス
    /// </summary>
    class ConsoleWindow : public IEditorWindow {
    public: // メンバ関数
        /// <summary>
        /// コンストラクタ
        /// デフォルトでウィンドウを表示状態にする
        /// </summary>
        ConsoleWindow();
        /// <summary>
        /// デストラクタ（特別な解放処理なし）
        /// </summary>
        ~ConsoleWindow() override = default;
        //====================================================
        // IEditorWindow インターフェース実装
        //====================================================
        /// <summary>
        /// ウィンドウの描画処理（毎フレーム呼ばれる）
        /// </summary>
        void Draw() override;
        /// <summary>
        /// ウィンドウ名を返す（ImGui表示名）
        /// </summary>
        const char* GetName() const override { return "System Console"; }
    };
}