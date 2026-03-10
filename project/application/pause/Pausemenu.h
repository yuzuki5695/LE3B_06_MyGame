#pragma once
#include<Object3d.h>
#include<Sprite.h>
#include <vector>
#include <memory>

// ポーズメニューの選択肢
enum class PauseCommand {
    Resume,     // ゲーム再開
	Operation,  // 操作説明
    GoToTitle,  // タイトルへ
    Turnback,   // 戻る 
    kCount,     // 要素数カウント用（常に最後に置く）
    None        // 選択中
};

// UIパーツごとの演出データをまとめた構造体
struct UIElement {
    std::unique_ptr<Sprite> sprite;
    Vector2 position;
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

    void keyUpdate();
    void UpdateArrowPositions();
    void ExecuteCommand();

    void UpdateOperationMove();
    void UpdateCommandPosition();

    //void Motion();
    //void StartPause();

    //void CalculateEaseSize(Sprite* sprite, const Vector2& baseSize, float frame, float maxFrame);
    //void CalculateEasePos(Sprite* sprite, const Vector2& targetPos, float startY, float frame, float maxFrame);

private: // メンバ変数
    bool isActive_;  // メニューがアクティブか
    std::unique_ptr <Sprite> paused_;
    // UIパーツをまとめて管理
    std::vector<UIElement> panels_;
    std::vector<Vector2> panelTargetPos_;

    PauseCommand command_ = PauseCommand::None;
    static const uint32_t kMaxCommand = static_cast<uint32_t>(PauseCommand::kCount);
    std::vector<UIElement> commands_[kMaxCommand];
    uint32_t selectedIndex_; // 現在選んでいる項目
    Vector2 commandOffset_[kMaxCommand];
    int commandPanelIndex_[kMaxCommand];
    Vector2 commandStart_[kMaxCommand];
    Vector2 commandEnd_[kMaxCommand];
    // 配置用定数
    const float kMenuTopY = 250.0f;     // 最初の項目のY座標
    const float kMenuIntervalY = 80.0f; // 項目間の間隔
    const float kArrowOffsetX = 170.0f; // 項目中心から見た矢印の右オフセット

    UIElement arrow_; // 選択中の項目を示す矢印UI


    bool isOperationMove_ = false;

    float operationTimer_ = 0.0f;
    const float operationDuration_ = 0.5f;

    static constexpr float kPanelCenterX = 640.0f;
    static constexpr float kPanelRightX = 1980.0f;
    static constexpr float kPanelLeftX = -1280.0f;
    Vector2 panel1Start_;
    Vector2 panel1End_;

    Vector2 panel2Start_;
    Vector2 panel2End_;
    bool isOperationMode_ = false;
    bool isCommandRunning_ = false; // コマンド実行中


    float frame_;           // 現在の経過フレーム
    const float kMaxFrame = 30.0f; // アニメーションにかける時間（30フレーム = 約0.5秒）   
    bool isFinished_ = false; // 閉じる演出が完全に終わった

    float panelStartOffsetY_;

    float openTimer_;
    bool isOpening_;
    bool isClosing_;
    static constexpr float kOpenMaxFrame = 20.0f;
    bool fige;
public: // メンバ関数 
    // シーン側から状況を確認するための関数
    PauseCommand GetCommand() const { return command_; }

    bool IsActive() const { return isActive_; }

    // ポーズメニューを起動する（または閉じる準備をする）
    void SetActive(bool active) {
        if (active) {
            isOpening_ = true;
            openTimer_ = 0.0f;
        }
        isActive_ = active;
    }

    bool IsOpening_() const { return isOpening_; }
};