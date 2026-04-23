#pragma once
#include <BaseCharacter.h>
#include <CollisionComponent.h>
#include <EnemyAttack.h>

namespace MyGame {
	/// <summary>
	/// プレイヤーキャラクタークラス
	/// </summary>
	class Enemy : public BaseCharacter {
	public:// メンバ関数
		// デストラクタ
		~Enemy() override;
		void Finalize();
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
		/// 衝突時の通知を受け取る
		/// </summary>
		void OnCollision(Collider* other);

	private: // メンバ変数
		CollisionComponent collision_;

		//std::unique_ptr<EnemyAttack> attack_; // 攻撃ロジックの保持
	public: // アクセッサ
		// getter
		//EnemyAttack* GetAttack() { return attack_.get(); }

		MyEngine::Transform* GetTransform() { return &object_->GetTransform(); }

	};
}