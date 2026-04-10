#include "FadeManager.h"
#include <NormalFade.h>
#ifdef min
#undef min
#endif
#include <algorithm>
#ifdef max
#undef max
#endif

namespace MyGame {
    // シングルトン用インスタンス
    std::unique_ptr<FadeManager> FadeManager::instance = nullptr;

    ///====================================================
    /// シングルトンインスタンスの取得
    ///====================================================
    FadeManager* FadeManager::GetInstance() {
        if (!instance) {
            instance = std::make_unique<FadeManager>();
        }
        return instance.get();
    }
    ///====================================================
    /// 終了処理
    ///====================================================
    void FadeManager::Finalize() {
        instance.reset();  // `delete` 不要
    }
    
    void FadeManager::Initialize() {

        
    }

    void FadeManager::StartFade(FadeType type, FadeStyle style, float duration) {
        type_ = type;
        duration_ = std::max(duration, 0.001f);
        timer_ = 0.0f;
        isFading_ = true;

        // 生成
        switch (style) {
        case FadeStyle::Normal:
            effect_ = std::make_unique<NormalFade>();
            break;
        case FadeStyle::SilhouetteSlide:
            // effect_ = std::make_unique<SlideFade>();
            break;
        case FadeStyle::SilhouetteExplode:
            //effect_ = std::make_unique<ExplodeFade>();
            break;
        }

        effect_->Initialize();
    }

    void FadeManager::Update() {
        if (!isFading_) return;

        timer_ += 1.0f / 60.0f;
        t_ = std::clamp(timer_ / duration_, 0.0f, 1.0f);

        effect_->Update(t_, type_);

        if (t_ >= 1.0f) {
            isFading_ = false;
        }
    }

    void FadeManager::Draw() {
        if (effect_) {
            effect_->Draw();
        }
    }
}