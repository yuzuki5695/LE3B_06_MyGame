#pragma once
#include<BaseEmitter.h>
#include<CameraManager.h>

class DefaultParticleEmitter : public BaseEmitter {
public:
    using BaseEmitter::BaseEmitter;


    void Update() override;


    bool exploded_ = false;
    float timer_ = 0.0f;
    float explodeDelay_ = 0.4f;  // 何秒後に爆発するか
public:
    // ===== 外部から設定用 =====
    void SetPosition(const Vector3& pos) { transform_.translate = pos; }
    void SetLifeTime(float time) { frequency_ = time; }
    void SetEmitCount(uint32_t count) { count_ = count; }
    void SetExplodeDelay(float sec) { explodeDelay_ = sec; }
    
    bool IsDead() const;

};

