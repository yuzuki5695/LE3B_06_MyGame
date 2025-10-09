#pragma once
#include <memory>
#include <DirectXCommon.h>
#include <SrvManager.h>
#include <CopylmageCommon.h>
#include <SpriteCommon.h>

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
    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
//    // 毎フレーム更新
//    void Update();
//    // 描画
//    void Draw();
//
//    //void StartFadeOut(float duration, bool useRenderTexture = true);
//    //void StartFadeIn(float duration, bool useRenderTexture = true);
//
//    //bool IsFading() const { return isFading_; }
//    //bool IsFadeEnd() const { return !isFading_; }


private:
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;

//    float timer_ = 0.0f;
//    float duration_ = 1.0f;
//    bool isFading_ = false;
//    bool useRenderTexture_ = true;
//    FadeType fadeType_ = FadeType::None;
//
//    //// スプライト版フェード用
//    //uint32_t spriteTex_ = 0;
//    //float spriteAlpha_ = 0.0f;
};