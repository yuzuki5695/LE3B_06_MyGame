#pragma once
#include <Vector3.h>
#include<Object3d.h>
#include<Transform.h>
#include<random>

class Enemy {
public:
	~Enemy();

	// 初期化
	void Initialize();
	// 毎フレーム更新
	void Update(const Vector3& playerPos);
	// 描画
	void Draw();
	
	void attachBullet(const Vector3& playerPos);
	// 弾リストを取得
	//onst std::vector<Bullet*>& GetBullets() const { return bullets_; }

private:
	// Object3d
	std::unique_ptr<Object3d> object;
    
	
	Vector3 position_;
    float radius_ = 1.0f; // 敵の半径
    bool active_ = true;

	// ランダムエンジン
	std::mt19937 randomEngine;

	//std::vector<Bullet*> bullets_;
	//float bulletTimer_ = 0.0f;                   // 経過時間
	//const float bulletInterval_ = 5.0f;         // 30秒ごとに弾を撃てる
	//bool canShoot_ = true;                       // 弾を撃てるかどうか

public:

    bool IsActive() const { return active_; }
    Vector3 GetPosition() const { return position_; }
    float GetRadius() const { return radius_; }
	void SetInactive() { active_ = false; }

};
