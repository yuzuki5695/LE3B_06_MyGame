#pragma once
#include <BaseCharacter.h>
#include <PlayerMove.h>
#include <PlayerData.h>

namespace MyGame {
	/// <summary>
	/// プレイヤーキャラクタークラス
	/// </summary>
	class Player : public BaseCharacter {
	public:// メンバ関数
		// デストラクタ
		~Player() override;
		/// <summary>
		/// 初期化処理 
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// 更新処理
		/// </summary>
		void Update() override;
		/// <summary>
		/// 描画処理
		/// </summary>
		void Draw() override;
	private:
		PlayerData data_; // プレイヤーのデータ構造体
		// 各種コンポーネント
		std::unique_ptr<PlayerMove> moveComponent_; // 移動ロジックの保持


	public:
		PlayerMove* GetMoveComponent() { return moveComponent_.get(); }
	};
}