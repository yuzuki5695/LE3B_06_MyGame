#pragma once
#include <BaseUI.h>

class Player; // 前方宣言

class GamePlayUI : public BaseUI {
public: // メンバ関数
    void Initialize() override;
    void Update() override;
    void Draw() override;
    // アニメーション開始トリガー
    void StartControlUIAnimation();
    void StartStageProgressUI();
    void SetPlayer(Player* player) { player_ = player; }
private: // プレイベートメンバ関数
    void UpdateStageProgressUI();

    // UI生成用の内部関数
    void CreateWASDUI(const Vector2& baseCenter, const Vector2& size, float keySpacing, float groupSpacing); 
    // 更新処理の細分化
    void UpdateControlUI();
    void UpdateControlUIAnimation();
private: // メンバ変数
    std::vector<std::unique_ptr<Sprite>> uis_;
    std::vector<Vector2> uiOriginalSizes_;
    Player* player_ = nullptr;
    bool isAnimating_;
    float timer_;
    float duration_;

      
    std::unique_ptr <Sprite> gage_;
    std::unique_ptr <Sprite> player_ui_;
    float uiStartRailLength_ = 0.0f;
    bool uiProgressStarted_ = false;
    bool uiProgressFinished_ = false;
};