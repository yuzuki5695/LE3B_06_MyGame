#pragma once
#include <BaseCharacter.h>
#include <Sprite.h>
#include <PlayerMove.h>
#include <PlayerData.h>
#include <PlayerReticle.h>
#include <PlayerAttack.h>
#include <PlayerDeath.h>

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

		void SyncWorldTransformByRail();

		void DrawImGui();

	private: // メンバ変数
		PlayerData data_; // プレイヤーのデータ構造体
		MyEngine::Transform transform_;
		// 各種コンポーネント
		std::unique_ptr<PlayerMove> move_;       // 移動ロジックの保持
		std::unique_ptr<PlayerReticle> reticle_; // レティクルロジックの保持
		std::unique_ptr<PlayerAttack> attack_;   // 攻撃ロジックの保持
		std::unique_ptr<PlayerDeath> death_;     // 死亡演出ロジックの保持

		std::unique_ptr <MyEngine::Sprite> targetreticle_; // レティクル用スプライト
		std::unique_ptr <MyEngine::Object3d> target_; // ターゲット用3Dオブジェクト
		MyEngine::Transform targettransform_; 
		MyEngine::Vector3 aimWorldPos_; // 
		// カメラの位置調整用オフセット
		MyEngine::Vector3 baseOffset_;
	public: // アクセッサ
		// getter
		PlayerMove* GetMove() { return move_.get(); }
		PlayerReticle* GetReticle() { return reticle_.get(); }
		PlayerAttack* GetAttack() { return attack_.get(); }    
		PlayerDeath* GetDeath() { return death_.get(); }    
		const MyEngine::Vector3& GetAimWorldPos() const { return aimWorldPos_; }

		void SetAimWorldPos(const MyEngine::Vector3& pos) { aimWorldPos_ = pos; }
		MyEngine::Sprite* GetSprite() { return targetreticle_.get(); }
		MyEngine::Object3d* GetTarget() { return target_.get(); }
			

		const MyEngine::Vector3& GetRotate() const { return object_->GetRotate(); }
		const MyEngine::Vector3& GetTranslate() const { return object_->GetTranslate(); }

		void SetTranslate(const MyEngine::Vector3& translate) { object_->SetTranslate(translate); }
		void SetRotate(const MyEngine::Vector3& rotate) { object_->SetRotate(rotate); }
	};
}