#pragma once
#include <BaseCharacter.h>
#include <ICollisionReceiver.h>
#include <OBBCollider.h>
#include <EnemyAttack.h>

namespace MyGame {
	/// <summary>
	/// プレイヤーキャラクタークラス
	/// </summary>
	class Enemy : public BaseCharacter, public ICollisionReceiver {
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


	private: // 内部処理

        void OnCollision(Collider* other) override;

	private: // メンバ変数

		std::unique_ptr<OBBCollider> collider_;

		//std::unique_ptr<EnemyAttack> attack_; // 攻撃ロジックの保持
	public: // アクセッサ
		// getter
		//EnemyAttack* GetAttack() { return attack_.get(); }

		MyEngine::Transform* GetTransform() { return &object_->GetTransform(); }

	};
}