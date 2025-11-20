#include "TitleCamera.h"
#include <algorithm>

void TitleCamera::Initialize()
{
    maincamera_ = std::make_unique<Camera>();
    maincamera_->SetTranslate({0, 5, -30});
    maincamera_->SetRotate({0, 0, 0});

    subcamera_  = std::make_unique<Camera>();
    subcamera_->SetTranslate({0, 10, -80});
    subcamera_->SetRotate({0, 0, 0});
}

void TitleCamera::StartEaseToMain()
{
    easing_ = true;
    easeTimer_ = 0.0f;
    startPos_ = subcamera_->GetTranslate();
    startRot_ = subcamera_->GetRotate();
    targetPos_ = maincamera_->GetTranslate();
    targetRot_ = maincamera_->GetRotate();
}
    
void TitleCamera::StartEaseToSub()
{
    easing_ = true;
    easeTimer_ = 0.0f;
    startPos_ = maincamera_->GetTranslate();
    startRot_ = maincamera_->GetRotate();
    targetPos_ = subcamera_->GetTranslate();
    targetRot_ = subcamera_->GetRotate();
}

void TitleCamera::Update(float dt)
{
    if (easing_)
    {
        easeTimer_ += dt;
        float t = std::clamp(easeTimer_ / easeTime_, 0.0f, 1.0f);
        float s = t * t * (3 - 2 * t); // Smoothstep

        Vector3 pos = startPos_ + (targetPos_ - startPos_) * s;
        Vector3 rot = startRot_ + (targetRot_ - startRot_) * s;

        maincamera_->SetTranslate(pos);
        maincamera_->SetRotate(rot);

        if (t >= 1.0f)
            easing_ = false;
    }

    maincamera_->Update();
    subcamera_->Update();
}