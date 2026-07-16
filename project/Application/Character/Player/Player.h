#pragma once
#include <BaseCharacter.h>
#include <Sprite.h>
#include <PlayerMove.h>
#include <PlayerData.h>
#include <PlayerReticle.h>
#include <PlayerAttack.h>
#include <PlayerDeath.h>
#include <Collider.h>

namespace MyGame {

	// 前方宣言
	class Enemy;

	/// <summary>
	/// プレイヤーキャラクタークラス
	/// </summary>
	class Player : public BaseCharacter {
	public:// メンバ関数
		// デストラクタ
		~Player() override;
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
		/// 2Dスプライトの描画処理
		/// </summary>	
		void DrawSprite();
		/// <summary>
		/// レール上の相対位置からワールド座標への変換を行い、Object3dのTransformを更新する
		/// </summary>
		void SyncWorldTransformByRail();
		/// <summary>
		/// ImGuiの描画処理
		/// </summary>
		void DrawImGui();
		/// <summary>
		/// 経験値獲得とレベルアップの処理
		/// </summary>
		/// <param name="exp">獲得する経験値</param>
		void GainExp(uint32_t exp);
		/// <summary>
		/// レベルアップの条件をチェックし、レベルアップが必要な場合はレベルと必要経験値を更新する
		/// </summary>
		void CheckLevelUp();
		/// <summary>
        /// ダメージを受ける処理
        /// </summary>
        /// <param name="damage">減少するHPの量</param>
        void ApplyDamage(uint32_t damage);
	private: // メンバ変数
		PlayerData data_; // プレイヤーのデータ構造体
		MyEngine::Transform transform_;   // プレイヤーのTransform
		MyEngine::Vector3 collidersize_;  // 当たり判定のサイズ
		// コライダー
		std::unique_ptr<Collider> collider_;
		// 各種コンポーネント
		std::unique_ptr<PlayerMove> move_;       // 移動ロジックの保持
		std::unique_ptr<PlayerReticle> reticle_; // レティクルロジックの保持
		std::unique_ptr<PlayerAttack> attack_;   // 攻撃ロジックの保持
		std::unique_ptr<PlayerDeath> death_;     // 死亡演出ロジックの保持
		std::unique_ptr <MyEngine::Sprite> targetreticle_; // レティクル用スプライト
		std::unique_ptr <MyEngine::Object3d> target_; // ターゲット用3Dオブジェクト
		// カメラの位置調整用オフセット
		MyEngine::Vector3 baseOffset_;
		// 参照ポインタ
		Enemy* enemy_;
		// 経験値・レベル用メンバ変数
		uint32_t kMaxLevel;      // 最大レベル
		uint32_t level_;         // 現在のレベル
		uint32_t exp_;           // 現在の経験値
		uint32_t nextLevelExp_; // 次のレベルに必要な経験値
		bool isStateUpdateEnabled_;
		bool isLevelUpRequested_;
		// HP関連のメンバ変数		
		uint32_t hp_;    // 現在のHP
        uint32_t maxHp_; // 最大HPの初期値
		// 無敵時間管理用
		bool isInvincible_;       // 無敵フラグ
		float invincibleTimer_;    // 無敵残り時間タイマー
		float kInvincibleTime; // 無敵時間の長さ
		// 点滅用のメンバ変数
		bool isVisible_;	   // 点滅状態の可視性フラグ
		float blinkTimer_;     // 点滅タイマー
		float kBlinkInterval_; // 点滅間隔
	public: // アクセッサ
		// getter
		PlayerMove* GetMove() { return move_.get(); }
		PlayerReticle* GetReticle() { return reticle_.get(); }
		PlayerAttack* GetAttack() { return attack_.get(); }
		PlayerDeath* GetDeath() { return death_.get(); }
		const MyEngine::Vector3& GetColliderSize() const { return collidersize_; }
		const MyEngine::Vector3& GetRotate() const { return object_->GetRotate(); }
		const MyEngine::Vector3& GetTranslate() const { return object_->GetTranslate(); }
		MyEngine::Vector3 GetExpTargetPosition() const;
		MyEngine::Vector3 GetWorldPosition() const { return object_->GetTranslate() + MyEngine::Vector3{ 0.0f, 1.0f, 0.5f }; }
		MyEngine::Sprite* GetSprite() { return targetreticle_.get(); }
		MyEngine::Object3d* GetTarget() { return target_.get(); }
		uint32_t GetLevel() const { return level_; }
		uint32_t GetMaxLevel() const { return kMaxLevel; }
		uint32_t GetExp() const { return exp_; }
		uint32_t GetNextLevelExp() const { return nextLevelExp_; }
		MyEngine::Vector3 GetForward() const;        
        int GetHP() const { return hp_; }
        int GetMaxHP() const { return maxHp_; }
		// setter
		void SetTranslate(const MyEngine::Vector3& translate) { object_->SetTranslate(translate); }
		void SetRotate(const MyEngine::Vector3& rotate) { object_->SetRotate(rotate); }
		void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
		void SetStateUpdateEnabled(bool enable) { isStateUpdateEnabled_ = enable; }
		bool ConsumeLevelUpRequest();
	};
}