#pragma once
#include <memory>
#include <vector>
#include <Sprite.h>

namespace MyGame {
    /// <summary>
    /// フェードの種類
    /// </summary>
    enum class FadeType {
        None,    // 通常
        FadeIn,  // フェードイン
        FadeOut, // フェードアウト
    };

    /// <summary>
    /// フェードの演出スタイル
    /// </summary>
    enum class FadeStyle {
        Normal,   // 全画面フェード
        SilhouetteExplode,    // 中心から外に飛び散って消えるフェード
        SilhouetteSlide,      // 左→右に消える、右→左に現れる（線状フェード）
    };
 
    struct FadeShape {
        std::unique_ptr<MyEngine::Sprite> mission_;
        MyEngine::Vector2 position;
        MyEngine::Vector2 size;
        float scale;
        float delay;
        float rotation;
        float rotationSpeed;
    };
}