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
			Ui::titile_01,Ui::titile_02,Ui::titile_03,Ui::titile_04,
			Ui::titile_03,Ui::titile_05,Ui::titile_06
		};

		for (const auto& t : textures) {
			TextureManager::GetInstance()->LoadTexture(t);
		}

		TextureManager::GetInstance()->LoadTexture(Ui::UI_02);

		titleSize_ = { 120.0f,100.0f };
		titleStartPos_ = { 1500.0f,70.0f };
		titleEndPos_ = { 650.0f,70.0f };
		const float stepX = 130.0f;

		for (uint32_t i = 0; i < (uint32_t)textures.size(); ++i) {
			TitleCharUI ch{};

			ch.mission_ = Sprite::Create(textures[i], titleStartPos_, 0.0f, titleSize_);
			ch.mission_->SetTextureSize(titleSize_);

			ch.delay = i * kInterval;
			if (i < 3)
				ch.offset = { stepX * i,0 };
			else
				ch.offset = { stepX * (i - 3),120 };

			titleChars_.push_back(std::move(ch));
		}


		TitleCharUI startUI{};

		Vector2 startPos = { 1300.0f, 450.0f };
		Vector2 endPos = { 750.0f, 450.0f };

		startUI.mission_ = Sprite::Create(Ui::UI_02, startPos, 0.0f, { 360.0f, 90.0f });
		// 専用座標使用
		startUI.useCustomPos = true;
		startUI.startPos = startPos;
		startUI.endPos = endPos;
		float lastTitleEndTime = (textures.size() - 1) * kInterval + kDuration;
		startUI.delay = lastTitleEndTime;
		titleChars_.push_back(std::move(startUI));
		// 初期化
		timer_ = 0.0f;
		isFinished_ = false;
	}

	void TitleUI::Update() {
		if (!isStarted_) {
			for (auto& Chars : titleChars_) {
				Chars.mission_->Update();
			}
			return;
		}
		timer_ += 1.0f / 60.0f;
		float lastEndTime = 0.0f;

		for (auto& ch : titleChars_) {
			float endTime = ch.delay + kDuration;
			lastEndTime = std::max(lastEndTime, endTime);
			float t = 0.0f;

			float localT = std::clamp((timer_ - ch.delay) / kDuration, 0.0f, 1.0f);	

			if (!PlayingOut) {
				// =========================
				// 登場
				// =========================
				float localT = std::clamp((timer_ - ch.delay) / kDuration, 0.0f, 1.0f);
				t = localT;
			} else {
				// =========================
				// 退場
				// =========================
								// delay無視
				float localT = std::clamp(timer_ / kDuration, 0.0f, 1.0f);
				t = 1.0f - localT;
			}

			float easeT = EaseOutCubic(t);

			UpdateChar(ch, easeT);
		}

		if (timer_ >= lastEndTime) {
			// 通常演出終了
			if (!PlayingOut) {
				isFinished_ = true;
			}

			// 逆演出終了
			else {
				isReverseFinished_ = true;
			}
		}

		for (auto& Chars : titleChars_) {
			Chars.mission_->Update();
		}
	}

	void TitleUI::Draw() {
		for (auto& Chars : titleChars_) {
			Chars.mission_->Draw();
		}
	}

	void TitleUI::UpdateChar(TitleCharUI& ch, float t) {

		// イージング
		float easeT = EaseOutBack(t);

		Vector2 start;
		Vector2 end;

		// 専用座標を使うUI
		if (ch.useCustomPos) {

			start = ch.startPos;
			end = ch.endPos;
		} else {

			// 通常タイトル文字
			start = titleStartPos_ + ch.offset;
			end = titleEndPos_ + ch.offset;
		}

		ch.mission_->SetPosition(Vector2::Lerp(start, end, easeT));
	}
}