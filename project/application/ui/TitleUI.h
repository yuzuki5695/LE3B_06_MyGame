#pragma once
#include <BaseUI.h>

class TitleUI : public BaseUI {
private:

    struct TitleChar {
        std::unique_ptr<Sprite> sprite;

        Vector2 startPos;   // 開始位置
        Vector2 endPos;     // 終了位置
        Vector2 offset;
        float delay;

        bool useCustomPos = false; // PressStartなど専用位置
    };
public:
    void Initialize() override;
    void Update() override;
    void Draw() override;


    void UpdateChar(TitleChar& ch, float t);
    /// <summary>
    /// 退場演出（逆再生）を開始する
    /// </summary>
    void StartExitAnimation();
private:
    std::vector<TitleChar> titleChars_;

    Vector2 titleSize_;
    Vector2 titleStartPos_;
    Vector2 titleEndPos_;

    float timer_ = 0.0f;

    float duration_ = 1.0f;
    float interval_ = 0.1f;

    // --- 退場アニメ ---
    float exitTimer_ = 0.0f;
    bool isExiting_ = false;

    const float kDuration = 1.0f;
    const float kInterval = 0.1f;

    bool isFinished_ = false;

public:

    void SetFinished(bool active) { isFinished_ = active; }
    bool IsFinished() const { return isFinished_; }
};