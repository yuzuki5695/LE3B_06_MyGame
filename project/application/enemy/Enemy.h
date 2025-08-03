#pragma once
#include <Vector3.h>
#include<random>
#include<Transform.h>
#include<BaseCharacter.h>
#include<Sprite.h>

class Player;

struct EnemySpawnTrigger {
    float zThreshold;
    int spawnCount;
    bool hasSpawned;
};


class Enemy : public BaseCharacter {
public:
	~Enemy() override;

	// 初期化
	void Initialize() override;	    
	void SetInitialize(float baseZ);

	// 毎フレーム更新
	void Update() override;
	// 描画
	void Draw() override;		
	// 弾の発射
	//void AttachBullet(const Vector3& playerPos);

	void SetPlayer(Player* player) { player_ = player; }
	
	void SetInactive() {
		if (!isDying_) {
			isDying_ = true; 
			deathTimer_ = 0.0f;
		}
	}
	
	// アクティブ状態の取得・設定
    bool IsActive() const { return isActive_ ; }
	void SetActive(bool active) { isActive_  = active; }

	void Kill() { isActive_ = false; isDead_ = true; }
    bool IsDead() const { return isDead_; }
	
	OBB GetOBB() const;

private:
	// ポインタ
    Player* player_; // プレイヤー
	Transform transform_;
	// Object3d
	std::unique_ptr<Object3d> object;

	float radius_ = 1.0f; // 敵の半径
	// ランダムエンジン
	std::mt19937 randomEngine;

	float bulletTimer_ = 0.0f;                   // 経過時間
	const float bulletInterval_ = 5.0f;         // 30秒ごとに弾を撃てる
	bool canShoot_ = true;                       // 弾を撃てるかどうか

	bool isDying_ = false;     // 死亡エフェクト中かどうか
	float deathTimer_ = 0.0f;  // 死亡経過時間
	const float deathDuration_ = 1.0f; // 1秒間でスケールを0に

	bool isActive_ = false;
    bool isDead_ = false;     // 倒されたか（削除対象か）

	float spawnBaseZ_ = 0.0f;  

public:
	float GetSpawnBaseZ() const { return spawnBaseZ_; }
};