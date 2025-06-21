#pragma once
#include <Vector3.h>
#include<random>
#include<Transform.h>
#include<Character.h>

class Player;

class Enemy : public Character {
public:
	~Enemy() override;

	// 初期化
	void Initialize() override;
	// 毎フレーム更新
	void Update() override;
	// 描画
	void Draw() override;
	

	void attachBullet(const Vector3& playerPos);

	void SetPlayer(Player* player) { player_ = player; }

private:	  
    Player* player_;

	// Object3d
	std::unique_ptr<Object3d> object;


	Vector3 position_;
	float radius_ = 1.0f; // 敵の半径
	bool active_ = true;

	// ランダムエンジン
	std::mt19937 randomEngine;

	float bulletTimer_ = 0.0f;                   // 経過時間
	const float bulletInterval_ = 5.0f;         // 30秒ごとに弾を撃てる
	bool canShoot_ = true;                       // 弾を撃てるかどうか



public:
};