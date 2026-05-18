#include "FadeManager.h"
#include <algorithm>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <NormalFade.h>
#include <SlideFade.h>
#include <ExplodeFade.h>
#include <SceneManager.h>

using namespace MyEngine;

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

    void FadeManager::Update() {
        if (!isFading_) return;

        timer_ += 1.0f / 60.0f;
        t_ = std::clamp(timer_ / duration_, 0.0f, 1.0f);

        effect_->Update(t_, type_);

        if (t_ >= 1.0f) {
			isFadeInFinished_ = true; // フェードインが完了したことを示すフラグを立てる
			isFading_ = false;      // フェード終了
            // フェード完了後にシーン切り替えが予約されている場合は実行
            if (isSceneChangeReserved_) {
                // シーンマネージャに切り替え処理を実行
                SceneManager::GetInstance()->ChangeScene(nextSceneName_);
                isSceneChangeReserved_ = false;
            }
        }
    }

    void FadeManager::Draw() {
        if (effect_) {
            effect_->Draw();
        }
    }

    void FadeManager::StartFade(FadeType type, FadeStyle style, float duration) {
        // すでにフェード中なら無視
        if (isFading_) {
            return;
        }
        type_ = type;
        duration_ = std::max(duration, 0.001f);
        timer_ = 0.0f;
        isFading_ = true;
        isFadeInFinished_ = false;
		// フェードスタイルに応じたエフェクトクラスのインスタンスを生成
        switch (style) {
		case FadeStyle::Normal: // 全画面フェード
            effect_ = std::make_unique<NormalFade>();
            break;
		case FadeStyle::SilhouetteSlide: // 左→右に消える、右→左に現れる（線状フェード）
            effect_ = std::make_unique<SlideFade>();
            break;
		case FadeStyle::SilhouetteExplode: // 中心から外に飛び散って消えるフェード
            effect_ = std::make_unique<ExplodeFade>();
            break;
        }
		// 現在のフェードエフェクトを初期化
        effect_->Initialize();
    }

    void FadeManager::SceneChangeFade(const std::string& sceneName, FadeStyle style, float duration) {
		// すでにフェード中またはシーン切り替えが予約されている場合は無視
        if (isFading_ || isSceneChangeReserved_) {
            return;
        }
        // フェード開始
        StartFade(FadeType::FadeOut, style, duration);
        // シーン切り替えを予約
        nextSceneName_ = sceneName;
        // フェード完了後にシーン切り替えを行うフラグを立てる
        isSceneChangeReserved_ = true;
    }
}