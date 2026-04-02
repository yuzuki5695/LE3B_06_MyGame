#pragma once
#include <IMenuComponent.h>

namespace MyEngine {
    /// <summary>
    /// 「設定」メニューを担当するクラス。
    /// </summary>
    class SettingsMenu : public IMenuComponent {
    public: // メンバ関数
        /// <summary>
        /// 設定メニューの描画
        /// </summary>
        /// <param name="LT">多言語テキスト取得ラムダ</param>
        void Render(const std::function<std::string(const std::string&)>& LT) override;
    };
}