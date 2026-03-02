#include <TitleSpriteMotion.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <Easing.h>
#include <FadeManager.h>
#include <TextureManager.h>
#include <algorithm>
#include <CameraManager.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace LoadResourceID;
using namespace Easing;

void TitleSpriteMotion::Initialize() {

    TextureManager::GetInstance()->LoadTexture("Textures/Ui/titile_01.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/titile_02.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/titile_03.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/titile_04.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/titile_05.png");
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/titile_06.png");     
    TextureManager::GetInstance()->LoadTexture("Textures/Ui/titile_07.png");
    std::vector<std::string> titleTextures =
    {
        "Textures/Ui/titile_01.png",
        "Textures/Ui/titile_02.png",
        "Textures/Ui/titile_03.png",
        "Textures/Ui/titile_04.png",
        "Textures/Ui/titile_05.png",
        "Textures/Ui/titile_06.png",
        "Textures/Ui/titile_07.png"
    };

    titleSize_ = { 120.0f, 100.0f };
    kTitleStartPos = { 1500.0f, 70.0f };
    kTitleEndPos = { 650.0f, 70.0f };          
    kDuration = 1.0f;     // 1文字のアニメ時間 
    kInterval = 0.1f;     // 出現間隔

    const int kSpriteCount = 7;    
    for (int i = 0; i < kSpriteCount; i++) {
        TitleSpriteData data;
        data.sprite = Sprite::Create(titleTextures[i], kTitleStartPos, 0.0f, titleSize_);
        data.sprite->SetTextureSize(titleSize_);

        kStep = { 130.0f, 0.0f };
        data.startTime = i * kInterval;   // 1秒ずつずらす

        data.Offset.x = i * kStep.x; 
        if (i >= 3) {
            data.Offset.y = 120.0f;
            data.Offset.x = (i - 2) * kStep.x; 
        } else {
            data.Offset.y = 0.0f;
        }
        titleSprites_.push_back(std::move(data));
    }
         
    kStartStartPos = { 1300.0f, 450.0f }; // 右から 
    kStartEndPos   = { 750.0f, 450.0f };
    ui_start_ = Sprite::Create(texture::Ui02, kStartStartPos, 0.0f, { 360.0f, 90.0f }); 
    ui_start_->SetTextureSize(Vector2{ 360.0f,90.0f });
    timer_ = 0.0f;

    isStartUIFinished_ = false;  
    isReverseFinished_ = false;
}

void TitleSpriteMotion::Update() {
    if (CameraManager::GetInstance()->GetMode() == CameraMode::Follow)
    {
        if (!isReverseActive_ && !isReverseFinished_) {
            isReverseFinished_ = true;
            isReverseActive_ = true;
            reverseTimer_ = 0.0f;
        }

        reverseTimer_ += 1.0f / 60.0f;

        float t = std::clamp(reverseTimer_ / kDuration, 0.0f, 1.0f);
        float easeT = EaseOutBack(1.0f - t);

        for (auto& data : titleSprites_)
        {
            Vector2 startPos = { kTitleStartPos.x + data.Offset.x,
                                 kTitleStartPos.y + data.Offset.y };

            Vector2 endPos = { kTitleEndPos.x + data.Offset.x,
                                 kTitleEndPos.y + data.Offset.y };

            Vector2 pos;
            pos.x = startPos.x + (endPos.x - startPos.x) * easeT;
            pos.y = startPos.y + (endPos.y - startPos.y) * easeT;

            data.sprite->SetPosition(pos);
            data.sprite->Update();
        }

        // ui_start_
        Vector2 pos;
        pos.x = kStartStartPos.x +
            (kStartEndPos.x - kStartStartPos.x) * easeT;
        pos.y = kStartStartPos.y;

        ui_start_->SetPosition(pos);
        ui_start_->Update();

        if (t >= 1.0f)
        {
            isReverseActive_ = false;
            isReverseFinished_ = true;   // 🔥 ここ重要
        }
    }

    if (!CameraManager::GetInstance()->GetTitleCamera()->GetIsFollowAllowed() && CameraManager::GetInstance()->GetMode() == CameraMode::Default) {
        timer_ += 1.0f / 60.0f;

        for (auto& data : titleSprites_) {
            if (timer_ < data.startTime)
                continue;
            float localTime = timer_ - data.startTime;
            float t = std::clamp(localTime / kDuration, 0.0f, 1.0f);
            float easeT = EaseOutBack(t);


            // 文字ごとの開始＆終了位置
            Vector2 startPos;
            startPos.x = kTitleStartPos.x + data.Offset.x;
            startPos.y = kTitleStartPos.y + data.Offset.y;

            Vector2 endPos;
            endPos.x = kTitleEndPos.x + data.Offset.x;
            endPos.y = kTitleEndPos.y + data.Offset.y;

            //  補間
            Vector2 pos;
            pos.x = startPos.x + (endPos.x - startPos.x) * easeT;
            pos.y = startPos.y + (endPos.y - startPos.y) * easeT;

            data.sprite->SetPosition(pos);

            data.sprite->Update();
        }

        int lastIndex = static_cast<int>(titleSprites_.size()) - 1;
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
                isStartUIFinished_ = true;   // 🔥 ここで完了
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
}

void TitleSpriteMotion::Draw2D() {
    for (auto& data : titleSprites_) {
        data.sprite->Draw();
    }

    ui_start_->Draw();
}