#pragma once
#include<BaseCharacter.h>
#include<Sprite.h>

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
	/// 描画更新
	/// </summary>
	void Draw() override;
	
	void DrawSprite();	

	// キーボードでの移動入力処理
	void MoveInput(float speed);	
	// ブースト状態更新
	void UpdateBoostState();
	// デバッグ用のImGui描画
	void DebugImgui();
	
	// 弾の発射
	void AttachBullet();
	
	void UpdateTargetPosition(Transform& targetTransform, float speed);


	void UpdateReticlePosition();

private:// メンバ変数
	std::unique_ptr <Object3d> object = nullptr;
	Transform transform_{};
	
	std::unique_ptr <Object3d> target_ = nullptr;
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
	
	// ブースト
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

public:// メンバ変数
	// getter
	// 参照を返す（変更不可）
	Object3d* GetPlayerObject() { return object.get(); }
	void SetTransform(const Transform& t) {
		transform_ = t;
	}

	// Transformのpositionを返すgetter
	Vector3 GetPosition() const {
		return transform_.translate;
	}
};