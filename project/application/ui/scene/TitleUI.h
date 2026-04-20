#pragma once
#include <BaseUI.h>
#include <Vector2.h>

namespace MyGame {
    /// <summary>
    /// タイトル用UI
    /// </summary>
    class TitleUI : public BaseUI {
    private:
        struct TitleCharUI {
            std::unique_ptr<MyEngine::Sprite> sprite;

            MyEngine::Vector2 startPos;   // 開始位置
            MyEngine::Vector2 endPos;     // 終了位置
            MyEngine::Vector2 offset;
            float delay;
            bool useCustomPos = false;
        };
    public: // メンバ関数
        void Initialize() override;
        void Update() override;
        void Draw() override;

        void UpdateChar(TitleCharUI& ch, float t);
        void Start() {
            isStarted_ = true;
            timer_ = 0.0f; // ←リスタートしたいなら必須
        }
        
        void StartReverse() {
            PlayingOut = true;
            isFinished_ = false;
            timer_ = 0.0f;
        }
        
        bool IsFinished() const {
            return PlayingOut == true && isFinished_ == true;
        }

    private: // メンバ変数
        std::vector<TitleCharUI> titleChars_;
        MyEngine::Vector2 titleSize_;
        MyEngine::Vector2 titleStartPos_;
        MyEngine::Vector2 titleEndPos_;
            
        const float kDuration = 1.0f;
        const float kInterval = 0.1f;     
        float timer_ = 0.0f;
        bool isFinished_ = false;
        
        bool isStarted_ = false;

        bool PlayingOut = false;


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