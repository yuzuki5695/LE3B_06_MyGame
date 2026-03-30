#include "TitleUI.h"
#include <TextureManager.h>
#include <Easing.h>
#include <MatrixVector.h>
#include <CameraManager.h>
#include <algorithm>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <Tools/AssetGenerator/engine/generated/ResourceID.h>

using namespace ResourceID;
using namespace Easing;
using namespace MatrixVector;

void TitleUI::StartExitAnimation()
{
    if (!isExiting_)
    {
        isExiting_ = true;
        exitTimer_ = 0.0f;
    }
}

void TitleUI::Initialize() {

    titleChars_.clear();
    isFinished_ = false;
    isExiting_ = false;

    TextureManager::GetInstance()->LoadTexture(texture::Ui::UI_02);

    const std::vector<std::string> textures = {
        texture::Ui::titile_01,
        texture::Ui::titile_02,
        texture::Ui::titile_03,
        texture::Ui::titile_04,
        texture::Ui::titile_03,
        texture::Ui::titile_05,
        texture::Ui::titile_06,
    };

    for (const auto& t : textures)
    {
        TextureManager::GetInstance()->LoadTexture(t);
    }

    titleSize_ = {120.0f,100.0f};

    titleStartPos_ = {1500.0f,70.0f};
    titleEndPos_   = {650.0f,70.0f};

    const float stepX = 130.0f;

    // ----------------------
    // タイトル文字生成
    // ----------------------

    for (int i = 0; i < (int)textures.size(); ++i) {

        TitleChar ch{};

        ch.sprite = Sprite::Create(textures[i], titleStartPos_, 0.0f, titleSize_);
        ch.sprite->SetTextureSize(titleSize_);

        ch.delay = i * kInterval;

        if (i < 3)
            ch.offset = {stepX * i,0};
        else
            ch.offset = {stepX * (i-3),120};

        titleChars_.push_back(std::move(ch));
    }

    // ----------------------
    // PressStart
    // ----------------------

    TitleChar startUI{};

    Vector2 startStart = {1300.0f,450.0f};
    Vector2 startEnd   = {750.0f,450.0f};

    startUI.sprite = Sprite::Create(texture::Ui::UI_02,startStart,0.0f,{360,90});
    startUI.sprite->SetTextureSize({360,90});

    startUI.startPos = startStart;
    startUI.endPos   = startEnd;

    startUI.useCustomPos = true;

    startUI.delay = textures.size() * kInterval + kDuration;

    titleChars_.push_back(std::move(startUI));

    timer_ = 0.0f;
}

void TitleUI::Update() {

    bool isReverse = isExiting_ ||
        (CameraManager::GetInstance()->GetMode() == CameraMode::Follow);

    // ----------------------
    // 退場アニメ
    // ----------------------

    if (isReverse) {

        exitTimer_ += 1.0f / 60.0f;

        float t = std::clamp(exitTimer_ / kDuration,0.0f,1.0f);
        float easeT = EaseInCubic(t);

        for (auto& ch : titleChars_) {

            Vector2 start;
            Vector2 end;

            if (ch.useCustomPos)
            {
                start = ch.endPos;
                end   = ch.startPos;
            }
            else
            {
                start = titleEndPos_ + ch.offset;
                end   = titleStartPos_ + ch.offset;
            }

            ch.sprite->SetPosition(Vector2::Lerp(start,end,easeT));
        }
    }

    // ----------------------
    // 登場アニメ
    // ----------------------

    else {

        if (!CameraManager::GetInstance()->GetTitleCamera()->GetIsFollowAllowed() &&
            CameraManager::GetInstance()->GetMode() == CameraMode::Default)
        {
            timer_ += 1.0f / 60.0f;

            for (auto& ch : titleChars_) {

                float localT = std::clamp((timer_ - ch.delay)/kDuration,0.0f,1.0f);
                float easeT  = EaseOutCubic(localT);

                UpdateChar(ch,easeT);
            }

            float lastEndTime = 0.0f;

            for (auto& ch : titleChars_)
            {
                float endTime = ch.delay + kDuration;
                lastEndTime = std::max(lastEndTime,endTime);
            }

            if (timer_ >= lastEndTime)
            {
                isFinished_ = true;
            }
        }
    }

    for (auto& ch : titleChars_)
    {
        ch.sprite->Update();
    }
}

void TitleUI::Draw() {

    for (auto& ch : titleChars_)
    {
        ch.sprite->Draw();
    }
}

void TitleUI::UpdateChar(TitleChar& ch,float t) {

    float easeT = EaseOutBack(t);

    Vector2 start;
    Vector2 end;

    if (ch.useCustomPos)
    {
        start = ch.startPos;
        end   = ch.endPos;
    }
    else
    {
        start = titleStartPos_ + ch.offset;
        end   = titleEndPos_ + ch.offset;
    }

    ch.sprite->SetPosition(Vector2::Lerp(start,end,easeT));
}