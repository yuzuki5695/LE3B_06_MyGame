#pragma once
#include<BaseCharacter.h>

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
	// キーボードでの移動入力処理
	void MoveInput(float speed);	
	// ブースト状態更新
	void UpdateBoostState();
	// デバッグ用のImGui描画
	void DebugImgui();
private:// メンバ変数
	std::unique_ptr <Object3d> object = nullptr;
	Transform transform_{};

	Vector3 moveOffset;
	Vector3 moveDelta{};
	
	// ブースト
	float normalSpeed_ = 0.2f;
	float boostSpeed_ = 0.5f;
	bool isBoosting_ = false;
	float boostTime_ = 0.0f;
	const float boostDuration_ = 0.4f; // ブースト持続時間
	bool isCoolingDown_ = false;
	float cooldownTime_ = 0.0f;
	const float cooldownDuration_ = 3.0f; // クールダウン時間	

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