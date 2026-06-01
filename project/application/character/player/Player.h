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

		void GainExp(uint32_t exp);
		void CheckLevelUp();

	private: // メンバ変数
		PlayerData data_; // プレイヤーのデータ構造体
		MyEngine::Transform transform_;
		MyEngine::Vector3 colliderSize_;  // 当たり判定のサイズ
		// コライダー
		std::unique_ptr<Collider> collider_;
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
		// 参照ポインタ
		Enemy* enemy_;
		// 経験値・レベル用メンバ変数
		uint32_t level_ = 1;         // 現在のレベル
		uint32_t exp_ = 0;           // 現在の経験値
		uint32_t nextLevelExp_ = 100; // 次のレベルに必要な経験値 (初期値)
		bool isStateUpdateEnabled_ = true;

	public: // アクセッサ
		// getter
		PlayerMove* GetMove() { return move_.get(); }
		PlayerReticle* GetReticle() { return reticle_.get(); }
		PlayerAttack* GetAttack() { return attack_.get(); }
		PlayerDeath* GetDeath() { return death_.get(); }
		const MyEngine::Vector3& GetColliderSize() const { return colliderSize_; }
		const MyEngine::Vector3& GetRotate() const { return object_->GetRotate(); }
		const MyEngine::Vector3& GetTranslate() const { return object_->GetTranslate(); }
		const MyEngine::Vector3& GetAimWorldPos() const { return aimWorldPos_; }
		MyEngine::Sprite* GetSprite() { return targetreticle_.get(); }
		MyEngine::Object3d* GetTarget() { return target_.get(); }
		uint32_t GetLevel() const { return level_; }
		uint32_t GetExp() const { return exp_; }
		uint32_t GetNextLevelExp() const { return nextLevelExp_; }
		// setter
		void SetAimWorldPos(const MyEngine::Vector3& pos) { aimWorldPos_ = pos; }
		void SetTranslate(const MyEngine::Vector3& translate) { object_->SetTranslate(translate); }
		void SetRotate(const MyEngine::Vector3& rotate) { object_->SetRotate(rotate); }
		void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
		void SetStateUpdateEnabled(bool enable) { isStateUpdateEnabled_ = enable; }
	};
}