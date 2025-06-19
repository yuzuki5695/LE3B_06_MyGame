#pragma once
#include "Object3d.h"
#include "Vector3.h"

class Bullet {
public:
    Bullet();
    ~Bullet();

    void Initialize(const Vector3& position, const Vector3& direction, float speed);
    void Update();
    void Draw();

private:
    std::unique_ptr <Object3d> object_ = nullptr;
    Vector3 position_;
    Vector3 velocity_;
    float radius_ = 0.5f; // 弾の半径
    bool active_ = false;
public:
    bool IsActive() const { return active_; }
    void SetActive(bool active) { active_ = active; }
    float GetRadius() const { return radius_; }

    void SetInactive() { active_ = false; }
    
    const Vector3& GetPosition() const { return position_; }
};
