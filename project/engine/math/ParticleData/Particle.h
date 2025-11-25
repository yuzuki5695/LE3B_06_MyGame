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
};