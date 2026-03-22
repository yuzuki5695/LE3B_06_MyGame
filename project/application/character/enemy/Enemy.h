#pragma once
#include <BaseCharacter.h>
#include <Player.h>

enum class EnemyMoveType {
    Stationary, // その場に固定（マップ配置）
    RailFollow  // レールカメラに同期（画面内に居座る）
};

/// <summary>
/// 敵キャラクタークラス
/// </summary>
class Enemy : public BaseCharacter {
public: // メンバ関数

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Enemy() override;
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update() override;
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() override;
    // プレイヤーの参照をセットする（向き計算用）
    void SetPlayer(Player* player) { player_ = player; }
    // 移動タイプの設定
    void SetMoveType(EnemyMoveType type) { moveType_ = type; }
    void SyncWithPlayerFront();

private: // メンバ変数
    std::unique_ptr<Object3d> object;// 敵モデルデータ
    Transform transform_;
    // 追加のメンバ
    Player* player_ = nullptr;
    EnemyMoveType moveType_;

    // レール同期用のオフセット（カメラから見てどの位置にいたいか）
    Vector3 railOffset_ = { 0.0f, 0.0f, 50.0f };

    // 内部計算メソッド
    void LookAtPlayer();
    void SyncWithRail();
};