#pragma once
#include <BaseUI.h>
#include <Vector2.h>

namespace MyGame {
    /// <summary>
    /// タイトル用UI
    /// </summary>
    class TitleUI : public BaseUI {
    public: // メンバ関数
        void Initialize() override;
        void Update() override;
        void Draw() override;

    private: // メンバ変数
        std::vector<std::unique_ptr<MyEngine::Sprite>> titleChars_;

        //std::vector<std::unique_ptr<MyEngine::Sprite>> uis_;
        //std::vector<MyEngine::Vector2> uiOriginalSizes_;
        //bool isAnimating_;
        //float timer_;
        //float duration_;

        //std::unique_ptr <MyEngine::Sprite> gage_;
        //std::unique_ptr <MyEngine::Sprite> player_ui_;

        //bool uiProgressStarted_ = false;
        //bool uiProgressFinished_ = false;
    };
}