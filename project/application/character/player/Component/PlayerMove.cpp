#include"PlayerMove.h"
#include <Input.h>
#include <algorithm>

using namespace MyEngine;

namespace MyGame {

	void PlayerMove::Update(MyEngine::Transform& transform, float speedMultiplier) {
		Input* input = Input::GetInstance();

		// --- 1. 入力値の取得 ---
		Vector3 moveInput = { 0, 0, 0 };
		if (input->Pushkey(DIK_A)) moveInput.x -= speedMultiplier;
		if (input->Pushkey(DIK_D)) moveInput.x += speedMultiplier;
		if (input->Pushkey(DIK_W)) moveInput.y += speedMultiplier;
		if (input->Pushkey(DIK_S)) moveInput.y -= speedMultiplier;
		// --- 2. 移動の実行と制限 ---
		relativePos_.x += moveInput.x;
		relativePos_.y += moveInput.y;
		transform.translate.x += moveInput.x;
		transform.translate.y += moveInput.y;
		ApplyConstraint();

		// 傾き計算
		// 入力がある場合は傾け、ない場合は緩やかに水平に戻す
		UpdateTilt(moveInput);
		// 傾きを反映
		transform.rotate.x = tiltX_;
		transform.rotate.z = tiltZ_;
	}

	void PlayerMove::ApplyConstraint() {
		// 画面外（レールの移動可能範囲）に出ないようにクランプ
		relativePos_.x = std::clamp(relativePos_.x, -10.0f, 10.0f);
		relativePos_.y = std::clamp(relativePos_.y, -2.5f, 8.5f);
	}

	void PlayerMove::UpdateTilt(const Vector3& moveInput) {
		const float tiltSpeed = 0.05f;   // 1フレームで傾く速度
		const float maxTilt = 0.4f;      // 最大傾斜角
		const float returnSpeed = 0.08f; // 入力がない時に戻る速度(0.0~1.0)

		// 横入力によるロール演出 (Z軸回転)
		if (moveInput.x < 0) {        // 左入力中
			tiltZ_ += tiltSpeed;
		} else if (moveInput.x > 0) { // 右入力中
			tiltZ_ -= tiltSpeed;
		} else {                      // 入力なし：中央に戻る
			tiltZ_ *= (1.0f - returnSpeed);
		}

		// 縦入力によるピッチ演出 (X軸回転)
		if (moveInput.y > 0) {        // 上入力中
			tiltX_ -= tiltSpeed;
		} else if (moveInput.y < 0) { // 下入力中
			tiltX_ += tiltSpeed;
		} else {                      // 入力なし：中央に戻る
			tiltX_ *= (1.0f - returnSpeed);
		}

		// 傾きの限界値を設定（端に押し付けている間もこの角度を維持する）
		tiltX_ = std::clamp(tiltX_, -maxTilt, maxTilt);
		tiltZ_ = std::clamp(tiltZ_, -maxTilt, maxTilt);	
	}
}