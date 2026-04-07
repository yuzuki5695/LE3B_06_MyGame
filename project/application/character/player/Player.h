#pragma once
#include <BaseCharacter.h>
#include <Sprite.h>
#include <PlayerMove.h>
#include <PlayerData.h>
#include <PlayerReticle.h>

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
		/// <summary>
		/// 2Dスプライトの描画処理
		/// </summary>	
		void DrawSprite();
	private:
		PlayerData data_; // プレイヤーのデータ構造体
		// 各種コンポーネント
		std::unique_ptr<PlayerMove> move_; // 移動ロジックの保持
		std::unique_ptr<PlayerReticle> reticle_; // 移動ロジックの保持

		std::unique_ptr <MyEngine::Sprite> targetreticle_; // レティクル用スプライト
		std::unique_ptr <MyEngine::Object3d> target_; // ターゲット用3Dオブジェクト
		MyEngine::Transform targettransform_;
	public:
		PlayerMove* GetMove() { return move_.get(); }
		PlayerReticle* GetReticle() { return reticle_.get(); }

		MyEngine::Sprite* GetSprite() { return targetreticle_.get(); }
		MyEngine::Object3d* GetTarget() { return target_.get(); }
	};
}