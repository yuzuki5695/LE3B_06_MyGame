#pragma once
#include <Vector3.h>
#include <Vector4.h>

class Object3d;

class PlayerDeath {
public:
    void Initialize();

    /// <summary>
    /// 死亡演出の更新
    /// </summary>
    /// <param name="rotate">プレイヤーの回転参照</param>
    /// <param name="offset">プレイヤーのオフセット参照</param>
    /// <param name="object">色を変えるためのオブジェクトポインタ</param>
    void Update(Vector3& rotate, Vector3& offset, Object3d* object);

    bool IsFinished() const { return timer_ >= kDuration; }

private:
    float timer_ = 0.0f;
    const float kDuration = 2.5f; // 演出時間（任意に調整してください）

    // 演出用定数
    const float kFallSpeedY = 0.02f;
    const float kFallSpeedZ = 0.2f;
    const float kBlinkSpeed = 50.0f;
};