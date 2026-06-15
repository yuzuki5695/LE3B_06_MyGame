#pragma once
#include <BaseCharacter.h>
#include <EnemyAttack.h>
#include <Collider.h>
#include <EnemyDeath.h>
#include <random>
#include <EnemyType.h>
#include <Player.h>
#include <GamePlayparticle.h>

namespace MyGame {
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
		std::unique_ptr<EnemyDeath> death_;   // 死亡演出の保持
		bool isSpawned_ = false; // 出現フラグ
		MyEngine::Vector3 colliderSize_;  // 当たり判定のサイズ
		// プレイヤーへの参照
		Player* player_ = nullptr;
		uint32_t expReward_; // 倒した時の経験値
		bool isExpGranted_; // 経験値付与済みフラグ
		bool isKilledByPlayer_;
		bool isDeathStarted_;

		EnemyType enemyType_; // 敵のタイプ
		std::mt19937 randomEngine; // 乱数生成器
		bool hasSpawnedDeathParticle_ = false;
	public: // アクセッサ
		bool IsSpawned() const { return isSpawned_; }
		bool IsExpGranted() const { return isExpGranted_; }
		bool IsKilledByPlayer() const { return isKilledByPlayer_; }
		bool IsDeathStarted() { return isDeathStarted_; }
		// getter
		MyEngine::Transform* GetTransform() { return &object_->GetTransform(); }
		EnemyAttack* GetAttack() { return attack_.get(); }
		EnemyDeath* GetDeath() { return death_.get(); }
		MyEngine::Vector3 GetColliderSize() const { return colliderSize_; }
		Player* GetPlayer() const { return player_; }
		uint32_t GetExpReward() const { return expReward_; }
		EnemyType GetEnemyType() { return enemyType_; }

		// setter
		void SetPlayer(Player* player) { player_ = player; }
		void SetSpawned(bool flag) { isSpawned_ = flag; }
		void SetExpGranted(bool flag) { isExpGranted_ = flag; }
		void SetKilledByPlayer(bool flag) { isKilledByPlayer_ = flag; }
		void SetDeathStarted(bool flag) { isDeathStarted_ = flag; }
		void SetColliderSize(const MyEngine::Vector3& size) {
			colliderSize_ = size;
			// すでにコライダーが生成されている場合は、そのサイズを即座に同期する
			if (collider_) { collider_->SetSize(colliderSize_); }
		}

		bool HasSpawnedDeathParticle() const { return hasSpawnedDeathParticle_; }
		void SetSpawnedDeathParticle(bool flag) { hasSpawnedDeathParticle_ = flag; }
	};
}