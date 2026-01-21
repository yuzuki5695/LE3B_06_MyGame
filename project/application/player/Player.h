#pragma once
#include<BaseCharacter.h>
#include<Sprite.h>
#include <ParticleEmitter.h>
#include<OBB.h>

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
    /// キーボード入力での移動処理
    /// </summary>
    /// <param name="speed">移動速度</param>
	void MoveInput(float speed);	
    /// <summary>
    /// ブースト状態の更新
    /// </summary>
	void UpdateBoostState();
    /// <summary>
    /// デバッグ用のImGui描画
    /// </summary>
	void DebugImgui();	
    /// <summary>
    /// 弾の発射処理
    /// </summary>
	void AttachBullet();
	/// <summary>
    /// ターゲットの更新
    /// </summary>
	void UpdateTargetPosition(Transform& targetTransform, float speed);
    /// <summary>
    /// レティクルの位置を更新
    /// </summary>
	void UpdateReticlePosition();	   
	/// <summary>
    /// プレイヤーを非アクティブ状態にする（無効化）
    /// </summary>
	void SetInactive() {
		active_ = false;
	}
    /// <summary>
    /// 当たり判定用を取得
    /// </summary>
	OBB GetOBB() const;
	// アクティブ状態の取得・設定
    bool IsActive() const { return active_; }
	void SetActive(bool isactive) { active_ = isactive; }
	/// <summary>
    /// 被弾エフェクトを開始
    /// </summary>    
	void StartHitEffect() {
		isHit_ = true;
		hitEffectTimer_ = 0.0f;
	}

	/// <summary>
	/// プレイヤー死亡演出の開始
	/// </summary>
	void StartDeathEffect();
	void SetDeadInactive() { isDeadEffectActive_ = true; }
	// アクティブ状態の取得・設定
    bool IsDead() const { return isDeadEffectActive_; }
	void SetDead_(bool isactive) { isDeadEffectActive_ = isactive; }
private:// メンバ変数
	bool active_ = true;
	std::unique_ptr <Object3d> object = nullptr;  // プレイヤーの3Dオブジェクト
	Transform transform_{};	
	std::unique_ptr <Object3d> target_ = nullptr; // ターゲット用3Dオブジェクト
	Transform targetpos_{};
	Vector3 copypos;
	std::unique_ptr <Sprite> targetreticle_ = nullptr; // レティクル用スプライト	
	Vector2 reticleScreenPos = { 640.0f, 360.0f }; // 画面中心 (例: 1280x720の解像度)
	// 球関連
	float bulletTimer_ = 0.0f;                   // 経過時間
	const float bulletInterval_ = 0.2f;         // 30秒ごとに弾を撃てる
	bool canShoot_ = true;                       // 弾を撃てるかどうか
	Vector3 moveOffset;
	Vector3 moveDelta{};	
	// ブースト関連
	float normalSpeed_ = 0.2f;
	float boostSpeed_ = 0.5f;
	bool isBoosting_ = false;
	float boostTime_ = 0.0f;
	const float boostDuration_ = 0.3f; // ブースト持続時間
	bool isCoolingDown_ = false;
	float cooldownTime_ = 0.0f;
	const float cooldownDuration_ = 3.0f; // クールダウン時間	
    Vector3 reticleWorldPos_;    // 3D空間のレティクル位置（ワールド座標）
    Vector2 reticleScreenPos_;   // 画面上のスプライト描画位置（スクリーン座標） 
	bool isHit_ = false;           // 弾に当たったことを示すフラグ
    float hitEffectTimer_ = 0.0f;  // 演出経過時間
    const float hitEffectDuration_ = 1.0f; // 演出にかける時間（秒）
	float previousTime_ = 0.0f;	    
	Vector4 originalColor_{};   // RGB + Alpha
	bool end = false; 
	std::unique_ptr <ParticleEmitter> circle_;
    RandomParameter random_;	
	bool isCharging_ = false;        // チャージ中か
    float chargeTime_ = 0.0f;        // 押し続けた時間
    const float maxChargeTime_ = 5.0f; // 最大チャージ時間（秒）
	Vector3 bulletOffsetLeft  = { -0.5f, 0.0f, 0.0f }; // 左側の発射位置
	Vector3 bulletOffsetRight = {  0.5f, 0.0f, 0.0f }; // 右側の発射位置	
	bool iskeyActive_ = false;       // ← プレイヤーが操作可能か
    bool isReticleVisible_ = false; // ← レティクル描画ON/OFF
    Vector3 relativePos_ = {0, 0, 0}; // カメラ内での相対位置（スクリーン座標的）

	
	// 死亡関連
	bool isDeadEffectActive_ = false;  // 死亡演出中フラグ
    float deathTimer_ = 0.0f;         // 死亡演出の経過時間
    float deathDuration_ = 2.0f;      // 演出全体の長さ（秒）
    Vector3 deathVelocity_ = {};      // 落下用の速度ベクトル
    Vector3 deathRotateSpeed_ = {};   // 回転スピード
	Vector3 deathStartPos_;

	    float deathFallSpeed_ = 0.5f; // 下方向に落ちるスピード

		
		Vector3 fallVelocity;
		
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
	bool IsDeadFinished() const { return end; }
	// Transformのpositionを返すgetter
	Vector3 GetPosition() const {
		return transform_.translate;
	}

    // Getter
    bool IsKeyActive() const { return iskeyActive_; }
    bool IsReticleVisible() const { return isReticleVisible_; }

    // Setter
    void SetKeyActive(bool active) { iskeyActive_ = active; }
    void SetReticleVisible(bool visible) { isReticleVisible_ = visible; }
};