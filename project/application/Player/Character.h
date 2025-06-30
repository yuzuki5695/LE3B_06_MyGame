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
    
    // アクティブ状態の取得・設定
    bool IsActive() const { return active_; }
	void SetInactive() { active_ = false; }

    // 位置の取得・設定
    Vector3 GetPosition() const { return position_; }
    void SetPosition(const Vector3& pos) { position_ = pos; }

protected: 
    bool active_ = true;
    Vector3 position_{};
};