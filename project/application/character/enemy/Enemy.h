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
	public:
		/// <summary>	
		/// 敵が誰によって死亡したかを表す列挙型
		/// </summary>
		enum class DeathType {
			None,   // 死亡していない、または未設定
			Player, // プレイヤーによって撃破された
			System  // システム（画面外消滅など）によって削除された
		};
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
		/// <summary>
		/// カメラの後方へ通過した敵を自動削除するか判定
		/// </summary>
		/// <returns>削除対象ならtrue</returns>
		bool ShouldAutoDestroy() const;
		/// <summary>
		/// 敵がカメラを通過したか判定
		/// </summary>
		/// <returns>通過済みならtrue</returns>
		bool HasPassedCamera() const;
	private: // メンバ変数
		// ポインタ
		std::unique_ptr<EnemyAttack> attack_; // 攻撃ロジックの保持
		std::unique_ptr<EnemyDeath> death_;   // 死亡演出の保持
		bool isSpawned_ = false;          // 出現フラグ
		MyEngine::Vector3 collidersize_;  // 当たり判定のサイズ
		Player* player_ = nullptr; 		  // プレイヤーへの参照
		uint32_t expreward_;              // 倒した時の経験値
		bool isExpGranted_;               // 経験値付与済みフラグ
		bool isKilledByPlayer_;           // プレイヤー撃破判定
		bool isDeathStarted_;             // 死亡演出開始フラグ
		EnemyType enemyType_;             // 敵のタイプ
		std::mt19937 randomEngine;        // 乱数生成器
		bool deathParticleRequested_;     // 死亡パーティクル生成要求
		DeathType deathType_;             // 敵の死亡させた対象
	public: // アクセッサ
		bool IsSpawned() const { return isSpawned_; }
		bool IsExpGranted() const { return isExpGranted_; }
		bool IsKilledByPlayer() const { return isKilledByPlayer_; }
		bool IsDeathStarted() { return isDeathStarted_; }
		// getter
		MyEngine::Transform* GetTransform() { return &object_->GetTransform(); }
		EnemyAttack* GetAttack() { return attack_.get(); }
		EnemyDeath* GetDeath() { return death_.get(); }
		MyEngine::Vector3 GetColliderSize() const { return collidersize_; }
		Player* GetPlayer() const { return player_; }
		uint32_t GetExpReward() const { return expreward_; }
		EnemyType GetEnemyType() { return enemyType_; }
		DeathType GetDeathType() const { return deathType_; }
		// setter
		void SetPlayer(Player* player) { player_ = player; }
		void SetSpawned(bool flag) { isSpawned_ = flag; }
		void SetExpGranted(bool flag) { isExpGranted_ = flag; }
		void SetKilledByPlayer(bool flag) { isKilledByPlayer_ = flag; }
		void SetDeathStarted(bool flag) { isDeathStarted_ = flag; }
		void SetColliderSize(const MyEngine::Vector3& size) {
			collidersize_ = size;
			// すでにコライダーが生成されている場合は、そのサイズを即座に同期する
			if (collider_) { collider_->SetSize(collidersize_); }
		}
		void SetDeathType(DeathType type) { deathType_ = type; }
		bool ConsumeDeathParticleRequest() {
			if (!deathParticleRequested_) return false;
			deathParticleRequested_ = false;
			return true;
		}
		void RequestDeathParticle() { deathParticleRequested_ = true; }
	};
}