#include "TitleUI.h"
#include <TextureManager.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <Easing.h>
#include <MatrixVector.h>
#include <CameraManager.h>
#include <algorithm>
#include <engine/math/Vector/Vector2.h>

using namespace LoadResourceID;
using namespace Easing;
using namespace MatrixVector;

void TitleUI::Initialize()
{
    titleChars_.clear();

    const std::vector<std::string> textures =
    {
        "Textures/Ui/titile_01.png",
        "Textures/Ui/titile_02.png",
        "Textures/Ui/titile_03.png",
        "Textures/Ui/titile_04.png",
        "Textures/Ui/titile_05.png",
        "Textures/Ui/titile_06.png",
        "Textures/Ui/titile_07.png"
    };

    for (const auto& t : textures)
    {
        TextureManager::GetInstance()->LoadTexture(t);
    }

    titleSize_ = { 120.0f, 100.0f };
    titleStartPos_ = { 1500.0f, 70.0f };
    titleEndPos_ = { 650.0f, 70.0f };

    duration_ = 1.0f;
    interval_ = 0.1f;

    const float stepX = 130.0f;

    for (int i = 0; i < (int)textures.size(); ++i)
    {
        TitleChar ch;

        ch.sprite = Sprite::Create(textures[i], titleStartPos_, 0.0f, titleSize_);
        ch.sprite->SetTextureSize(titleSize_);

        ch.delay = i * interval_;

        if (i < 3)
        {
            ch.offset = { stepX * i, 0.0f };
        } else
        {
            ch.offset = { stepX * (i - 3), 120.0f };
        }

        titleChars_.push_back(std::move(ch));
    }
 
    // Press Start
    startStartPos_ = { 1300.0f, 450.0f };
    startEndPos_ = { 750.0f, 450.0f };


    timer_ = 0.0f;
    reverseTimer_ = 0.0f;
    
    kStartStartPos = { 1300.0f, 450.0f }; // 右から 
    kStartEndPos   = { 750.0f, 450.0f };
    ui_start_ = Sprite::Create(texture::Ui02, kStartStartPos, 0.0f, { 360.0f, 90.0f }); 
    ui_start_->SetTextureSize(Vector2{ 360.0f,90.0f });
    timer_ = 0.0f;

}
void TitleUI::Update() {
    // カメラの状態、または明示的な退場フラグで逆再生を判定
    bool isReverse = isExiting_ || (CameraManager::GetInstance()->GetMode() == CameraMode::Follow);

    if (isReverse) {
        // --- 退場（逆再生）処理 ---
        exitTimer_ += 1.0f / 60.0f;
        // 1.0から0.0に向かうように計算
        float globalT = 1.0f - std::clamp(exitTimer_ / kDuration, 0.0f, 1.0f);

        for (size_t i = 0; i < titleChars_.size(); ++i) {
            // 各文字に少しずつ遅延をかけて消す場合はここで調整
            UpdateChar(titleChars_[i], globalT);
        }
    } else {
        if (!CameraManager::GetInstance()->GetTitleCamera()->GetIsFollowAllowed() && CameraManager::GetInstance()->GetMode() == CameraMode::Default) {
            // --- 登場（通常再生）処理 ---
            timer_ += 1.0f / 60.0f;

            for (size_t i = 0; i < titleChars_.size(); ++i) {
                // 文字ごとの遅延時間を考慮して進捗(0.0～1.0)を計算
                float charDelay = i * kInterval;
                float localT = std::clamp((timer_ - charDelay) / kDuration, 0.0f, 1.0f);

                float easeT = Easing::EaseOutCubic(localT);
                UpdateChar(titleChars_[i], easeT);
            }
        }
    }
     
    if (isReverse) {
        float t = std::clamp(reverseTimer_ / kDuration, 0.0f, 1.0f);
        float easeT = EaseOutBack(1.0f - t);
        // ui_start_
        Vector2 pos;
        pos.x = kStartStartPos.x + (kStartEndPos.x - kStartStartPos.x) * easeT;
        pos.y = kStartStartPos.y;
        ui_start_->SetPosition(pos);
        ui_start_->Update();
    }

    if (!CameraManager::GetInstance()->GetTitleCamera()->GetIsFollowAllowed() && CameraManager::GetInstance()->GetMode() == CameraMode::Default) {
        timer_ += 1.0f / 60.0f;

        int lastIndex = static_cast<int>(titleChars_.size()) - 1;
        float lastEndTime = (lastIndex * kInterval) + kDuration;

        if (!isStartUIActive_ && timer_ >= lastEndTime)
        {
            isStartUIActive_ = true;
            startUITimer_ = 0.0f;
        }
        if (isStartUIActive_)
        {
            startUITimer_ += 1.0f / 60.0f;

            float t = std::clamp(static_cast<float>(startUITimer_ / kStartDuration), 0.0f, 1.0f);
            if (t >= 1.0f)
            {
                t = 1.0f;
                //isStartUIFinished_ = true;   // 🔥 ここで完了
            }

            float easeT = EaseOutBack(t);

            Vector2 pos;
            pos.x = kStartStartPos.x +
                (kStartEndPos.x - kStartStartPos.x) * easeT;
            pos.y = kStartStartPos.y;

            ui_start_->SetPosition(pos);

        }
        ui_start_->Update();
    }
    // スプライト自体の更新
    for (auto& ch : titleChars_) {
        ch.sprite->Update();
    }
}
void TitleUI::Draw() {
    for (auto& ch : titleChars_)
    {
        ch.sprite->Draw();
    }

    ui_start_->Draw();
}

void TitleUI::UpdateChar(TitleChar& ch, float t) {
    float easeT = EaseOutBack(t);

    Vector2 start = titleStartPos_ + ch.offset;
    Vector2 end = titleEndPos_ + ch.offset;

    ch.sprite->SetPosition(Vector2::Lerp(start, end, easeT));
    ch.sprite->Update();
}