#pragma once
#include <Vector4.h>
#include <Velocity.h>

struct RandomParameter {
	// ランダムな速度の範囲
	Vector3 offsetMin;
	Vector3 offsetMax;
	// ランダムな回転の範囲
	Vector3 rotateMin;
	Vector3 rotateMax;
	// ランダムなスケールの範囲
	Vector3 scaleMin;
	Vector3 scaleMax;
	// ランダムな色の範囲
	float colorMin; // 最小値
	float colorMax; // 最大値
	// ランダムな寿命の範囲を追加
	float lifetimeMin;
	float lifetimeMax;
	// ランダムな速度の範囲を追加
	Velocity velocityMin;
	Velocity velocityMax;
};