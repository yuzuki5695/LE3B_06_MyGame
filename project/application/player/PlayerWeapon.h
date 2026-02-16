#pragma once
#include <Vector3.h>
#include <memory>

// 前方宣言
struct Transform;
class GamePlayCamera;

class PlayerWeapon {
public:
    void Initialize();
    
    /// <summary>
    /// 更新と発射チェック
    /// </summary>
    /// <param name="playerWorldPos">自機のワールド座標</param>
    /// <param name="targetWorldPos">レティクルのワールド座標</param>
    /// <param name="gameCam">ゲームプレイカメラ（前方ベクトル取得用）</param>
    void Update(const Vector3& playerWorldPos, const Vector3& targetWorldPos, GamePlayCamera* gameCam);

private:
    float bulletTimer_ = 0.0f;
    const float bulletInterval_ = 0.15f; // Player.hの規定値に合わせる
    bool canShoot_ = true;
};