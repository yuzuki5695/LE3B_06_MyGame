#pragma once
#include <Transform.h>
#include <string>

// 武器用データ構造体
struct PlayerWeaponData {
    float bulletInterval; // 発射間隔
    float bulletSpeed;    // 弾速（現在はPlayerBullet側にあるかもしれませんが、ここで管理すると調整が楽です）
};

struct PlayerMoveData {
    float speed;
    float maxX;
    float minY;
    float maxY;
    float tiltSpeed;
    float maxTilt;
    float returnSpeed;
    float dashDuration;
	float dashCoolDown;
	float dashSpeedMult;
};

struct PlayerReticleData {
    float speed;            // レティクルの移動速度
    float maxX;             // 左右の移動制限
    float minX;             
    float minY;             // 下方向の制限
    float maxY;             // 上方向の制限
    float forwardDistance;  // プレイヤーからどれくらい前方に配置するか
};

struct PlayerData {
    std::string fileName;
    Transform transform;
    PlayerMoveData move;
    PlayerReticleData reticle;
    PlayerWeaponData weapon;
};

