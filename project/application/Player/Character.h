#pragma once
#include <memory>
#include <Object3d.h>

class Character {
public:
    virtual ~Character() = default;

    // 基本処理
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    // 位置の取得・設定
    Vector3 GetPosition() const { return transform_.translate; }
    void SetPosition(const Vector3& pos) { transform_.translate = pos; }
    
    Vector3 GetRadius() const { return transform_.rotate; }
    void SetRadius(const Vector3& pos) { transform_.rotate = pos; }

protected: 
    Transform transform_{};
};