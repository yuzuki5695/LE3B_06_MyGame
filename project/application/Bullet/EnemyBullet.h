#pragma once
#include <BaseBullet.h>

class Object3d;

class EnemyBullet : public BaseBullet {
public:
    EnemyBullet();
    ~EnemyBullet() override;
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;
    void Initialize(const Vector3& startPos, const Vector3& targetPos, float speed) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() override;


private:
    std::unique_ptr<Object3d> object_;
    Vector3 position_;
    Vector3 size_;
    Vector3 direction_;
    Vector3 velocity_;
    
    int time = 0;
    int Maxtime = 500;

public:
    // 位置の取得・設定
    Vector3 GetPosition() const { return position_; }
    void SetPosition(const Vector3& pos) { position_ = pos; }
    
    Vector3 GetRadius() const { return size_; }
    void SetRadius(const Vector3& pos) { size_ = pos; }
};