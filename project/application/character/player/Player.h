#pragma once
#include <BaseCharacter.h>
#include <PlayerMove.h>

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
		std::unique_ptr<PlayerMove> moveComponent_; // 移動ロジックの保持



	public:
		PlayerMove* GetMoveComponent() { return moveComponent_.get(); }
	};
}