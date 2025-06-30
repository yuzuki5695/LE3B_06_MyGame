#pragma once
#include <fstream>
#include <vector>
#include <string>
#include<Transform.h>
#include<Character.h>

class Player: public Character{
public:
	~Player() override;

	// 初期化
	void Initialize() override;
	// 毎フレーム更新
	void Update() override;
	// 描画
	void Draw() override;
	// 弾の発射
	void AttachBullet();
	// デバッグ用のImGui描画
	void debugimgui();
	// キーボードでの移動入力処理
	void MoveInput(float speed);
	
	void UpdateTargetPosition(Transform& targetTransform, float speed);
private:
	// Object3d
	std::unique_ptr <Object3d> object = nullptr;
	Transform transform{};
	Vector3 moveOffset;
	Vector3 moveDelta{};
		
	std::unique_ptr <Object3d> target_ = nullptr;
	Transform targetpos_{};
	Vector3 copypos;


	float bulletTimer_ = 0.0f;                   // 経過時間
	const float bulletInterval_ = 0.5f;         // 30秒ごとに弾を撃てる
	bool canShoot_ = true;                       // 弾を撃てるかどうか

public: // メンバ関数
    // getter類は必要なら維持
    Transform GetTransform() const { return transform; }
    Object3d* GetObject3d() { return object.get(); }
    Vector3 GetForwardDirection() const { return Vector3(0.0f, 0.0f, 1.0f); }

};