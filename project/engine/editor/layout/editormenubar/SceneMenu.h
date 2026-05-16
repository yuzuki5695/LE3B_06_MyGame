#pragma once
#include <IMenuComponent.h>

namespace MyEngine {
    /// <summary>
	/// 「シーン」メニューを担当するクラス。
    /// </summary>
    class SceneMenu : public IMenuComponent {
	public: // メンバ関数
        /// <summary>
		/// シーンメニューの描画
        /// </summary>
        /// <param name="LT"></param>
        void Render(const std::function<std::string(const std::string&)>& LT) override;
    };
}