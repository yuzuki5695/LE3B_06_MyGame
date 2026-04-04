#pragma once
#include <BaseCharacter.h>

namespace MyGame {
	/// <summary>
	/// プレイヤーキャラクタークラス
	/// </summary>
	class Player : public BaseCharacter {
	public:// メンバ関数
		// デストラクタ
		~Player() override;
		void Initialize() override;
		void Update() override;
		void Draw() override;

	private:

	};
}