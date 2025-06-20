#pragma once
#include "BaseBullet.h"
#include "Vector3.h"
#include <memory>

class Object3d;

class PlayerBullet : public BaseBullet {
public:
    PlayerBullet();
    ~PlayerBullet() override;
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
 
    /// 弾のダメージ処理
    void Damage(int value) override;
private:
    std::unique_ptr<Object3d> object_;
    Vector3 position_;
    Vector3 direction_;
    Vector3 velocity_;
    bool active_ = true;
    int hp_ = 1;  // 初期HP（例）
public:
	// ===== 共通属性のためのアクセサ ===== //
	/// 弾が有効かどうか
    bool IsActive() const override { return active_; }
    void SetActive(bool active) override { active_ = active; }
	/// 弾のHPを取得・設定
    int GetHP() const override { return hp_; }
    void SetHP(int hp) override { hp_ = hp; }
};