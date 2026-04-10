#pragma once
#include <FadeTypes.h>
namespace MyGame {
	// フェードエフェクトのインターフェース
    class IFadeEffect {
	public: // メンバ関数
        virtual ~IFadeEffect() = default;

        virtual void Initialize() = 0;
        virtual void Update(float t, FadeType type) = 0;
        virtual void Draw() = 0;
    };
}