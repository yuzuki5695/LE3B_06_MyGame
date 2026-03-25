#pragma once
#include <Transform.h>
#include <string>

// 死亡演出のデータ構造体
struct PlayerDeathData {
    float duration;      // 演出の合計時間
    float fallSpeedY;    // 下方向への落下速度
    float fallSpeedZ;    // 前方向への慣性移動速度
    float blinkSpeed;    // 点滅の速さ
    float blinkThreshold; // 点滅判定の閾値 (0.0~1.0)
    float rotateSpeedX;  // 基本回転速度X
    float rotateSpeedZ;  // 基本回転速度Z
    float shakeAmount;   // 揺らぎの強さ
    float shakeCycleX;    // sin(timer_ * 0.5f) の 0.5f 部分
    float shakeCycleZ;    // cos(timer_ * 0.4f) の 0.4f 部分
    float shakeAmountMultZ; // (data_.shakeAmount * 0.75f) の 0.75f 部分
};

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
    PlayerDeathData death;
};

