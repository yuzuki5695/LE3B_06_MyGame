#pragma once
#include <Velocity.h>
#include <Transform.h>
#include <Vector4.h>

// パーティクル
struct Particle {
	Transform transform;
	Velocity Velocity;
	float lifetime;
	float currentTime;
	Vector4 color;
    Vector4 startColor; // ← 追加：発生時の元色を保持する
};