#pragma once
#include<Object3d.h>
#include <fstream>
#include <vector>
#include <string>
#include<Transform.h>

class Player{
public:
	~Player();

	// 初期化
	void Initialize();
	// 毎フレーム更新
	void Update();
	// 描画
	void Draw();
	// 弾の発射
	void AttachBullet();
	// デバッグ用のImGui描画
	void debugimgui();

	void MoveInput(float speed);
	void UpdateTargetPosition(Transform& targetTransform, float speed);
private:
	// Object3d
	std::unique_ptr <Object3d> object = nullptr;
	Transform transform{};
	Vector3 moveOffset;
	Vector3 moveDelta = { 0.0f, 0.0f, 0.0f };
		
	std::unique_ptr <Object3d> target_ = nullptr;
	Transform targetpos_{};
	Vector3 copypos;


	float bulletTimer_ = 0.0f;                   // 経過時間
	const float bulletInterval_ = 0.5f;         // 30秒ごとに弾を撃てる
	bool canShoot_ = true;                       // 弾を撃てるかどうか
	
	bool isActive_ = true; // 初期状態ではアクティブ

public: // メンバ関数
	Transform GetTransform() const { return transform; }

	Object3d* GetObject3d() { return object.get(); }
	Vector3 GetForwardDirection() const {
		// プレイヤーの正面方向ベクトル（例：Y軸方向に前向きの場合）
		// 必要に応じて回転を考慮したベクトル計算に変えてください
		return Vector3(0.0f, 0.0f, 1.0f);
	}
};