#pragma once
#include <IEvent.h>
#include <Sprite.h>

/// <summary>
/// スプライトの役割を定義する列挙型
/// </summary>
enum class SpriteID  {
    Background,
    TopBar,
    BottomBar,
    Mission
};

/// <summary>
/// スプライトと座標、アルファ値をまとめた構造体
/// </summary>
struct SpriteUnit {
    std::unique_ptr<MyEngine::Sprite> sprite_;
    MyEngine::Vector2 position;
    float alpha = 1.0f;
    SpriteID  id;
};

namespace MyGame {
    /// <summary>
    /// ゲーム開始イベントクラス
    /// </summary>
    class StartEvent : public IEvent {
    public: // メンバ関数
        /// <summary>
		/// 初期化処理
        /// </summary>
        void Initialize() override;
        /// <summary>
		/// 更新処理
        /// </summary>
        void Update() override;
        /// <summary>
		/// 2D描画処理
        /// </summary>
        void Draw2D() override;
        /// <summary>
		/// スプライトのIDを指定して構造体を返す関数
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        SpriteUnit* FindSprite(SpriteID id);
    private: // メンバ変数
        // スプライトの構造体のリスト
        std::vector<SpriteUnit> sprites_;
        // イベントの進行段階
        Event::EventPhase phase_ = Event::EventPhase::Phase1;

        float missionTimer_ = 0.0f;               // 経過時間
        float progress_ = 0.0f;       // 0.0 ~ 1.0 の進行率
        float phaseDuration_ = 1.0f;  // 現在のフェーズの目標時間（秒）
        bool timefige_ = false;
        float closeSpeed_ = 17.0f;

    };
}