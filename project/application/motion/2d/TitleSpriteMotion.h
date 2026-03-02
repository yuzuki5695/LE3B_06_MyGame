#pragma once
#include <Sprite.h>
    
struct TitleSpriteData
    {
        std::unique_ptr<Sprite> sprite;
        float startTime;   // 出現開始時間
        Vector2 Offset;     // Zずらし
    };


/// <summary>
/// タイトル演出クラス
/// </summary>
class TitleSpriteMotion {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    void Update();
    /// <summary>
    /// 描画処理
    /// </summary>
  //  void Draw3D();
    void Draw2D();

    bool GetisStartUIFinished_() const { return isStartUIFinished_; }

private: // メンバ変数
    std::vector<TitleSpriteData> titleSprites_;
    float timer_ = 0.0f;
    float kDuration;     // 1文字のアニメ時間    
    float kInterval;     // 出現間隔
    Vector2 kStep;



    std::unique_ptr<Sprite> ui_start_;
    bool isStartUIActive_ = false;
    float startUITimer_ = 0.0f;
    const float kStartDuration = 0.5f;


    // 座標設定
    Vector2 kTitleStartPos{}; // 左から
    Vector2 kTitleEndPos{};
    Vector2 kStartStartPos{  }; // 右から
    Vector2 kStartEndPos{ };

    Vector2 titleSize_;
    bool isStartUIFinished_;    // 🔥 完了フラグ


    bool isReverseActive_ = false;
    float reverseTimer_ = 0.0f;
    bool isReverseFinished_;
};