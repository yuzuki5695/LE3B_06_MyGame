#pragma once
#include <BaseCharacter.h>
#include <Player.h>
#include <EnemyData.h>
#include <EnemyWeapon.h>

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
    // プレイヤーの参照をセットする
    void SetPlayer(Player* player) { player_ = player; }
    // 移動タイプの設定
   // void SetMoveType(EnemyMoveType type) { moveType_ = type; }
    void UpdateDash();

private:
    // 内部計算メソッド
    //void LookAtPlayer(); // 未実装
    void SyncWithRail();
private: // メンバ変数
    std::unique_ptr<Object3d> object;// 敵モデルデータ    	
    EnemyData data_; // 敵のデータ構造体
    Transform transform_;
    // 追加のメンバ
    Player* player_ = nullptr;
     std::unique_ptr<EnemyWeapon> weapon_;
     
     //EnemyMoveType moveType_;

    //// レール同期用のオフセット（カメラから見てどの位置にいたいか）
    //Vector3 railOffset_ = { 0.0f, 0.0f, 50.0f };

   Vector3 dashVelocity_;        // 突撃時の速度ベクトル
   float dashRotationSpeed_ = 0.5f; // 突撃時の高速回転速度
   
   uint32_t das = 0;

    float chargeTimer_= 0.0f;
    float chargeRotation_ = 0.0f;    // 溜め中の回転角度
    Vector3 startChargePos_;        // 溜め開始時の座標


    float spawnTimer_ = 0.0f;
    float spawnDuration_ = 2.0f; // 1秒演出
    Vector3 spawnStartPos_;
    Vector3 spawnTargetPos_;
};