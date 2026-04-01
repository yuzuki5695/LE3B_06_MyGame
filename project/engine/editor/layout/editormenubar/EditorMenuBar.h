#pragma once
#include <SettingsMenu.h>
#include <ObjectMenu.h>

namespace MyEngine {
    namespace Editor {
        /// <summary>
        /// エディタ上部メニューバー描画クラス
        /// UI描画責務のみを持つ
        /// </summary>
        class MenuBar {
        public: // メンバ関数     
            /// <summary>
            /// メニューバー全体の描画処理
            /// </summary>
            /// <param name="LT">
            /// 多言語文字列取得ラムダ。
            /// 各メニューコンポーネントへそのまま渡される。
            /// </param>
            void Render(const std::function<std::string(const std::string&)>& LT);
        private: // メンバ変数
            SettingsMenu settingsMenu_; /// 「設定」メニューコンポーネント
            ObjectMenu objectMenu_;     /// 「オブジェクト」メニューコンポーネント
        public: // アクセッサ   
            ObjectMenu* GetObjectMenu() { return &objectMenu_; }
        };
    }
}