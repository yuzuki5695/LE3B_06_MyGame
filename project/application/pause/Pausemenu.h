#pragma once
#include<Object3d.h>
#include<Sprite.h>
#include <vector>
#include <memory>

// ポーズメニューの選択肢
enum class PauseCommand {
    Resume,     // ゲーム再開
    GoToTitle,  // タイトルへ
    kCount,     // 要素数カウント用（常に最後に置く）
    None        // 選択中
};

// UIパーツごとの演出データをまとめた構造体
struct UIElement {
    std::unique_ptr<Sprite> sprite;
    Vector2 size;   // サイズ
};

/// <summary>
/// 各演出を管理するクラス
/// </summary>
class Pausemenu {
public: // メンバ関数 
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();

    void CalculateEaseSize(Sprite* sprite, const Vector2& baseSize, float frame, float maxFrame);
    void UpdateSelection();  // キー入力による選択項目の変更
    void UpdateArrowPositions();
private: // メンバ変数
    std::unique_ptr <Sprite> ui_;
    // 選択肢の表示用（必要に応じて増やす）
   // std::vector<Sprite*> cursor_;
    PauseCommand command_ = PauseCommand::None;
    uint32_t selectedIndex_; // 現在選んでいる項目
    uint32_t MAXselectedIndex_;
    float frame_;           // 現在の経過フレーム
    const float kMaxFrame = 30.0f; // アニメーションにかける時間（30フレーム = 約0.5秒）
    Vector2 baseSize_; // 最終的なサイズ

    bool isActive_;   // 今メニューを開こうとしているか
    bool isFinished_ = false; // 閉じる演出が完全に終わった

    UIElement arrow_;

    // 列挙型から要素数を取得
    static const uint32_t kMaxCommand = static_cast<uint32_t>(PauseCommand::kCount);
    UIElement commands_[kMaxCommand];
    // 配置用定数
    const float kMenuTopY = 200.0f;     // 最初の項目のY座標
    const float kMenuIntervalY = 80.0f; // 項目間の間隔
    const float kArrowOffsetX = 120.0f; // 項目中心から見た矢印の右オフセット
public: // メンバ関数 
    // シーン側から状況を確認するための関数
    PauseCommand GetCommand() const { return command_; }


    bool IsActive() const { return isActive_; }
    // ポーズメニューを起動する（または閉じる準備をする）
    void SetActive(bool active) {
        isActive_ = active;
        isFinished_ = false; // 状態が変わるたびにリセット
    }
    bool IsFinished() const { return isFinished_; }
};