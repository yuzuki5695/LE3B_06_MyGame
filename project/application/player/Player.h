#pragma once
#include <BaseCharacter.h>
#include <Sprite.h>
#include <ParticleEmitter.h>
#include <OBB.h>
#include <PlayerMove.h>
#include <PlayerReticle.h>
#include <PlayerWeapon.h>
#include <PlayerDeath.h>

// プレイヤーの状態を定義
enum class State {
	None,
	Alive,      // 通常時（操作可能）
	Dead,       // 死亡演出中
	Goal,       // ゴール到達後
	Event,      // イベント中（操作不能・自動移動など）
};

/// <summary>
/// プレイヤーキャラクタークラス
/// </summary>
class Player : public BaseCharacter {
public:// メンバ関数
	// デストラクタ
	~Player() override;
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 3Dモデルの描画更新
	/// </summary>
	void Draw() override;
	/// <summary>
	/// 2Dスプライトの描画処理
	/// </summary>	
	void DrawSprite();
	/// <summary>
	/// デバッグ用のImGui描画
	/// </summary>
	void DebugImgui();
	/// <summary>
	/// 当たり判定用を取得
	/// </summary>
	OBB GetOBB() const;
	/// <summary>
    /// カメラのレール移動に合わせてプレイヤーのワールド座標を更新する
    /// </summary>
    void SyncWorldTransformByRail();

    // ... 省略 ...
    void SetState(State state) { state_ = state; }
	State GetState() { return state_; }
	
	// --- 各ステートの専用更新関数 ---
    void UpdateAlive();
    void UpdateDead();
	void UpdateGoal();
	void UpdateEvent();

private:// メンバ変数	
	// 各機能をクラスのインスタンス
	std::unique_ptr<PlayerMove> move_;          // プレイヤ―の移動制御クラス
	std::unique_ptr<PlayerReticle> reticle_;    // プレイヤ―の照準制御クラス
	std::unique_ptr<PlayerWeapon> weapon_;      // プレイヤ―の武器制御クラス
	std::unique_ptr<PlayerDeath> death_;        // プレイヤ―の死亡演出クラス
	State state_ = State::None; // 初期状態は生存

	std::unique_ptr <Object3d> object;  // プレイヤーの3Dオブジェクト
	Transform transform_{};
	std::unique_ptr <Object3d> target_; // ターゲット用3Dオブジェクト
	Transform targettransform_{};
	std::unique_ptr <Sprite> targetreticle_; // レティクル用スプライト	
	// 死亡演出用のオフセット（落下や回転のため）
	Vector3 deathOffset_;

public:// メンバ変数
	// getter
	// 参照を返す（変更不可）
	Object3d* GetPlayerObject() { return object.get(); }
	void SetTransform(const Transform& t) {
		transform_ = t;
		object->SetTranslate(transform_.translate);
		object->SetRotate(transform_.rotate);
		object->SetScale(transform_.scale);
	}

	// Transformのpositionを返すgetter
	Vector3 GetPosition() const {
		return transform_.translate;
	}
};