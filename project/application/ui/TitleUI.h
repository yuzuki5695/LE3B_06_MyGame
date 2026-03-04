#pragma once
#include <BaseUI.h>

class TitleUI : public BaseUI {
private:

    struct TitleChar
    {
        std::unique_ptr<Sprite> sprite;
        Vector2 offset;
        float delay;
    };
public:
    void Initialize() override;
    void Update() override;
    void Draw() override;


    void UpdateChar(TitleChar& ch, float t);
    /// <summary>
    /// 退場演出（逆再生）を開始する
    /// </summary>
    void StartExitAnimation() { isExiting_ = true; }
private:

    std::vector<TitleChar> titleChars_;
    std::vector<std::unique_ptr<Sprite>> uis_;

    Vector2 titleSize_;
    Vector2 titleStartPos_{}; // 左から
    Vector2 titleEndPos_{};
    Vector2 startStartPos_{}; // 右から
    Vector2 startEndPos_{};

    float timer_;
    float reverseTimer_;
    float  duration_;
    float interval_;

    float delay = 0.0f;      // 出現遅延
    float duration = 1.0f;   // 個別アニメ時間

    // --- アニメーション用変数 ---
    float exitTimer_ = 0.0f;
    bool isExiting_ = false; // 退場中フラグ

    // 定数（TitleSpriteMotionから移植）
    const float kDuration = 1.0f;
    const float kInterval = 0.1f;



    Vector2 kStartStartPos{}; // 右から
    Vector2 kStartEndPos{};

    
    std::unique_ptr<Sprite> ui_start_;
    bool isStartUIActive_ = false;
    float startUITimer_ = 0.0f;
    const float kStartDuration = 0.5f;
};
