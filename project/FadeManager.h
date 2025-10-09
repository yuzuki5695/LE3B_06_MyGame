#pragma once
#include <memory>
#include <DirectXCommon.h>
#include <SrvManager.h>
#include <CopylmageCommon.h>
#include <SpriteCommon.h>
#include <Sprite.h>

enum class FadeType {
    None,
    FadeIn,
    FadeOut,
    SpriteFade,
};

// フェードマネージャー
class FadeManager {
private:
    static std::unique_ptr<FadeManager> instance;
public:        
    FadeManager() = default;
    // 終了
    static FadeManager* GetInstance();
    // 初期化
    void Initialize();
    // 毎フレーム更新
    void Update();
    // 描画
    void Draw();

    void StartFadeIn(float duration = 1.0f);
    void StartFadeOut(float duration = 1.0f);
    bool IsFading() const { return isFading_; }
    bool IsFadeEnd() const { return !isFading_; }
    FadeType GetFadeType() const { return fadeType_; }


private:
    std::unique_ptr<Sprite> sprite_;
    FadeType fadeType_ = FadeType::None;
    bool isFading_ = false;
    float timer_ = 0.0f;
    float duration_ = 1.0f;
    float alpha_ = 0.0f;
};