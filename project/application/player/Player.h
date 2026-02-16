#pragma once
#include<BaseCharacter.h>
#include<Sprite.h>
#include <ParticleEmitter.h>
#include<OBB.h>
#include<PlayerMove.h>
#include<PlayerReticle.h>
#include<PlayerWeapon.h>

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
	/// <summary>
	/// プレイヤー死亡演出の開始
	/// </summary>
	void StartDeathEffect();
	void SetDeadInactive() { isDeadEffectActive_ = true; }
	// アクティブ状態の取得・設定
	bool IsDead() const { return isDeadEffectActive_; }
	void SetDead_(bool isactive) { isDeadEffectActive_ = isactive; }

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
	State state_ = State::None; // 初期状態は生存

	std::unique_ptr <Object3d> object = nullptr;  // プレイヤーの3Dオブジェクト
	Transform transform_{};
	std::unique_ptr <Object3d> target_ = nullptr; // ターゲット用3Dオブジェクト
	Transform targettransform_{};
	std::unique_ptr <Sprite> targetreticle_ = nullptr; // レティクル用スプライト	
	Vector2 reticleScreenPos = { 640.0f, 360.0f }; // 画面中心 (例: 1280x720の解像度)

	// 死亡関連
	bool isDeadEffectActive_ = false;  // 死亡演出中フラグ
	float deathTimer_ = 0.0f;         // 死亡演出の経過時間
	float deathDuration_ = 2.0f;      // 演出全体の長さ（秒）
	Vector3 deathVelocity_ = {};      // 落下用の速度ベクトル
	Vector3 deathRotateSpeed_ = {};   // 回転スピード
	Vector3 deathStartPos_;
	float deathFallSpeed_ = 0.5f; // 下方向に落ちるスピード
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