#include "TitleUI.h"
#include <TextureManager.h>
#include <Easing.h>
#include <algorithm>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Textures;
using namespace Easing;

namespace MyGame {
	void TitleUI::Initialize() {

		const std::vector<std::string> textures = {
		Ui::titile_01,
		Ui::titile_02,
		Ui::titile_03,
		Ui::titile_04,
		Ui::titile_05,
		Ui::titile_06,
		Ui::titile_07,
		};
		for (const auto& t : textures) {
			TextureManager::GetInstance()->LoadTexture(t);
		}

		titleSize_ = { 120.0f,100.0f };
		titleStartPos_ = { 1500.0f,70.0f };
		titleEndPos_ = { 650.0f,70.0f };
		const float stepX = 130.0f;

		for (uint32_t i = 0; i < (uint32_t)textures.size(); ++i) {
			TitleCharUI ch{};

			ch.sprite = Sprite::Create(textures[i], titleStartPos_, 0.0f, titleSize_);
			ch.sprite->SetTextureSize(titleSize_);

			ch.delay = i * kInterval;
			if (i < 3)
				ch.offset = { stepX * i,0 };
			else
				ch.offset = { stepX * (i - 3),120 };

			titleChars_.push_back(std::move(ch));
		}
	}

	void TitleUI::Update() {
		if (!isStarted_) {
			for (auto& Chars : titleChars_) {
				Chars.sprite->Update();
			}
			return;
		}
		timer_ += 1.0f / 60.0f;
		for (auto& ch : titleChars_) {

			float localT = std::clamp((timer_ - ch.delay) / kDuration, 0.0f, 1.0f);
			float easeT = EaseOutCubic(localT);

			UpdateChar(ch, easeT);
		}

		float lastEndTime = 0.0f;

		for (auto& ch : titleChars_)
		{
			float endTime = ch.delay + kDuration;
			lastEndTime = std::max(lastEndTime, endTime);
		}

		if (timer_ >= lastEndTime)
		{
				isFinished_ = true;
		}

		for (auto& Chars : titleChars_) {
			Chars.sprite->Update();
		}
	}

	void TitleUI::Draw() {
		for (auto& Chars : titleChars_) {
			Chars.sprite->Draw();
		}
	}

	void TitleUI::UpdateChar(TitleCharUI& ch, float t) {

		float easeT = EaseOutBack(t);

		Vector2 start;
		Vector2 end;

		if (ch.useCustomPos) {
			start = ch.startPos;
			end = ch.endPos;
		} else {
			start = titleStartPos_ + ch.offset;
			end = titleEndPos_ + ch.offset;
		}
		ch.sprite->SetPosition(Vector2::Lerp(start, end, easeT));
	}
}