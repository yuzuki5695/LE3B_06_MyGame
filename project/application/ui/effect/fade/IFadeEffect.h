#pragma once
#include <FadeTypes.h>

namespace MyGame {
	// フェードエフェクトのインターフェース
    class IFadeEffect {
	public: // メンバ関数
        virtual ~IFadeEffect() = default;
        /// <summary>
		/// 初期化処理
        /// </summary>
        virtual void Initialize() = 0;
        /// <summary>
		/// 更新処理
        /// </summary>
        /// <param name="t"></param>
        /// <param name="type"></param>
        virtual void Update(float t, FadeType type) = 0;
        /// <summary>
		/// 描画処理
        /// </summary>
        virtual void Draw() = 0;
    };
}