#pragma once
#include <Vector4.h>
#include <Matrix4x4.h>
#include <Transform.h>

namespace MyEngine {
	struct Velocity {
		Vector3 translate;
		Vector3 rotate;
		Vector3 scale;
	};

	// インスタンスデータ
	struct InstanceData
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	// パーティクル
	struct Particle {
		Transform transform;
		Velocity Velocity;
		float lifetime;
		float currentTime;
		Vector4 color;
		Vector4 startColor; // 発生時の元色を保持する
		bool useGravity = false;
	};
}