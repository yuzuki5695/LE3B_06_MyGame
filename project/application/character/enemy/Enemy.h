#pragma once
#include <BaseCharacter.h>
#include <EnemyAttack.h>
#include <Collider.h>

namespace MyGame {

	// 前方宣言
	class Player;
	
	/// <summary>
	/// 敵キャラクタークラス
	/// </summary>
	class Enemy : public BaseCharacter {
	public:// メンバ関数
		// デストラクタ
		~Enemy() override;
		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize() override;
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
	private: // メンバ変数
		// ポインタ
		std::unique_ptr<EnemyAttack> attack_; // 攻撃ロジックの保持
		std::unique_ptr<Player> player_;

	public: // アクセッサ
		// getter
		MyEngine::Transform* GetTransform() { return &object_->GetTransform(); }
		EnemyAttack* GetAttack() { return attack_.get(); }
		void SetPlayer(Player* player) { player_.reset(player); }
	};
}