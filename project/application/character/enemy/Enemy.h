#pragma once
#include <BaseCharacter.h>
#include <EnemyAttack.h>
#include <Collider.h>

namespace MyGame {
	/// <summary>
	/// プレイヤーキャラクタークラス
	/// </summary>
	class Enemy : public BaseCharacter {
	public:// メンバ関数
		// デストラクタ
		~Enemy() override;
		/// <summary>
		/// 
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

		//std::unique_ptr<EnemyAttack> attack_; // 攻撃ロジックの保持
	public: // アクセッサ
		// getter
		//EnemyAttack* GetAttack() { return attack_.get(); }
		   
		std::unique_ptr<Collider> collider_;

		MyEngine::Transform* GetTransform() { return &object_->GetTransform(); }

	};
}