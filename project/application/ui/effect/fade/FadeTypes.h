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
        Normal,               // 全画面フェード
        SilhouetteExplode,    // 中心から外に飛び散って消えるフェード
        SilhouetteSlide,      // 左→右に消える、右→左に現れる（線状フェード）
    };
    /// <summary>
	/// フェードの形状情報を保持する構造体
    /// </summary>
    struct FadeShape {
		std::unique_ptr<MyEngine::Sprite> sprite_;   // フェード用スプライト
		MyEngine::Vector2 position;                  // フェードの中心座標
		MyEngine::Vector2 size;                      // フェードのサイズ
		float scale;                                 // フェードの拡大率
		float delay;                                 // フェード開始までの遅延時間
		float rotation;                              // フェードの回転角度
		float rotationSpeed;                         // フェードの回転速度
    };
}