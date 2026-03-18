#pragma once
#include <Vector3.h>
#include <memory>

// 前方宣言
struct Transform;
class GamePlayCamera;

/// <summary>
/// プレイヤーの武器（弾発射）管理クラス
/// 発射間隔（リロード）の制御と、ターゲットに向けた弾の生成を行う。
/// </summary>
class PlayerWeapon {
public:
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    
    /// <summary>
    /// 毎フレームの更新と発射入力のチェック
    /// </summary>
    /// <param name="playerWorldPos">自機の現在のワールド座標</param>
    /// <param name="targetWorldPos">レティクル（照準）のワールド座標</param>
    /// <param name="gameCam">メインカメラ（発射方向の算出に使用）</param>
    void Update(const Vector3& playerWorldPos, const Vector3& targetWorldPos, GamePlayCamera* gameCam);
private:
    // --- 連射制御関連 ---
    float bulletTimer_ = 0.0f;           // 次の弾が打てるまでの経過時間
    const float bulletInterval_ = 0.15f; // 発射間隔
    bool canShoot_ = true;               // 現在発射可能かどうかのフラグ
};