#pragma once
#include <BaseCharacter.h>
#include <Sprite.h>
#include <ParticleEmitter.h>
#include <OBB.h>
#include <PlayerMove.h>
#include <PlayerReticle.h>
#include <PlayerWeapon.h>
#include <PlayerDeath.h>
#include <PlayerData.h>
#include <PlayerState.h>

// プレイヤーの状態を定義
enum class State {
	None,
	Alive,      // 通常時（操作可能）
	Dead,       // 死亡演出中
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
	//OBB GetOBB() const;
	/// <summary>
    /// カメラのレール移動に合わせてプレイヤーのワールド座標を更新する
    /// </summary>
    void SyncWorldTransformByRail();

    // ... 省略 ...
    void SetState(State state) { state_ = state; }
	State GetState() { return state_; }
	
private:// メンバ変数	
	// 各機能をクラスのインスタンス
	std::unique_ptr<PlayerMove> move_;          // プレイヤ―の移動制御クラス
	std::unique_ptr<PlayerReticle> reticle_;    // プレイヤ―の照準制御クラス
	std::unique_ptr<PlayerWeapon> weapon_;      // プレイヤ―の武器制御クラス
	std::unique_ptr<PlayerDeath> death_;        // プレイヤ―の死亡演出クラス
	State state_ = State::None; // 初期状態

	std::unique_ptr <Object3d> object;  // プレイヤーの3Dオブジェクト
	PlayerData data_; // プレイヤーのデータ構造体

	Transform transform_{};

	std::unique_ptr <Object3d> target_; // ターゲット用3Dオブジェクト
	Transform targettransform_{};
	std::unique_ptr <Sprite> targetreticle_; // レティクル用スプライト	
	// 死亡演出用のオフセット（落下や回転のため）
	Vector3 deathOffset_;
	std::unique_ptr<IPlayerState> activeState_; // 現在の状態
public:// メンバ変数
	// getter
	// 参照を返す（変更不可）
	Object3d* GetPlayerObject() { return object.get(); }
	Sprite* GetTargetReticle() { return targetreticle_.get(); }
	void SetTransform(const Transform& t) {
		data_.transform = t;
		object->SetTranslate(data_.transform.translate);
		object->SetRotate(data_.transform.rotate);
		object->SetScale(data_.transform.scale);
	}

	// Transformのpositionを返すgetter
	Vector3 GetPosition() const {
		return data_.transform.translate;
	}
	Vector3 GetForward();
	Vector3 GetWorldPosition()const { return object->GetWorldPosition(); }
	void ChangeState(std::unique_ptr<IPlayerState> newState) {
		if (activeState_) activeState_->Exit(this);
		activeState_ = std::move(newState);
		activeState_->Enter(this);
	}

	// 各機能へのゲッター（Stateクラスから触れるようにする）
    PlayerMove* GetMove() { return move_.get(); }
	PlayerReticle* GetReticle() { return reticle_.get(); }
	PlayerWeapon* GetWeapon() { return weapon_.get(); }
	PlayerDeath* GetDeath() { return death_.get(); }
	// Stateクラスからアクセスするためのゲッター
    Transform& GetTransform() { return transform_; }	   
	Vector3& GetRotate() { return transform_.rotate; }
	Vector3& GetDeathOffset() { return deathOffset_; }
	Transform& GetTarGetTransform() { return targettransform_; }
	Object3d* GetTarget() { return target_.get(); }

};