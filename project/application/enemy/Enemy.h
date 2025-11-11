#pragma once
#include <Vector3.h>
#include<random>
#include<Transform.h>
#include<BaseCharacter.h>
#include<Sprite.h>
#include<OBB.h>

// ヘッダーの重複を防ぐ前方宣言
class Player;

/// <summary>
/// 敵の移動タイプを定義する列挙体
/// </summary>
enum class MoveType {
    None,       // 移動しない
    Vertical,   // 縦方向に移動
    Horizontal  // 横方向に移動
};

/// <summary>
/// 敵の出現条件（トリガー）を定義する構造体
/// </summary>
struct EnemySpawnTrigger {
    Vector3 Position;   // 出現位置
    int spawnCount;     // 同時に出現させる数
    bool hasSpawned;    // すでに出現済みかどうか
    MoveType moveType;  // 敵の移動タイプ
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
    /// 出現時の初期化処理
    /// </summary>
    /// <param name="baseZ">出現位置の基準となるZ座標</param>
    /// <param name="moveType">移動タイプ（縦・横など）</param>
    void SetInitialize(float baseZ, MoveType moveType);
	/// <summary>
    /// 更新処理
    /// </summary>
	void Update() override;	    
	/// <summary>
    /// 描画処理
    /// </summary>
	void Draw() override;		
    /// <summary>
    /// 弾の発射処理
    /// </summary>
    /// <param name="playerPos">プレイヤーの現在座標</param>
	void AttachBullet(const Vector3& playerPos);
    /// <summary>
    /// プレイヤー情報を設定する
    /// </summary>
	void SetPlayer(Player* player) { player_ = player; }	    
	/// <summary>
    /// 敵が死んだ判定へ移行
    /// </summary>
	void SetInactive() {
		if (!isDying_) {
			isDying_ = true; 
			deathTimer_ = 0.0f;
		}
	}
    /// <summary>
    /// 現在のアクティブ状態を取得
    /// </summary>
    bool IsActive() const { return isActive_; }
    /// <summary>
    /// アクティブ状態を設定
    /// </summary>
    void SetActive(bool active) { isActive_ = active; }
    /// <summary>
    /// 強制的に敵を削除（非アクティブ化）
    /// </summary>
    void Kill() { isActive_ = false; }
    /// <summary>
    /// 死亡判定を取得
    /// </summary>
    bool IsDead() const { return isDead_; }
    /// <summary>
    /// 敵のOBB（当たり判定）を取得
    /// </summary>
    OBB GetOBB() const;
    /// <summary>
    /// 新しい座標と移動タイプを設定
    /// </summary>
    /// <param name="pos">新しい座標</param>
    /// <param name="moveType">移動タイプ</param>
    void SetnewTranslate(const Vector3& pos, MoveType moveType);
private: // メンバ変数
	// ポインタ
    Player* player_; // プレイヤー
	Transform transform_; //プレイヤーのtransform
	std::unique_ptr<Object3d> object;// 敵モデルデータ
    float radius_ = 1.0f;                    // 当たり判定半径
    float spawnBaseZ_ = 0.0f;                // 出現Z位置の基準  
    //====================================================
    // ランダム挙動関連
    //====================================================
    std::mt19937 randomEngine;               // 乱数生成器
    std::uniform_real_distribution<float> moveDirDist_; // 移動方向（-1.0〜1.0）
    Vector3 moveDirection_;                  // 現在の移動方向
    float moveSpeed_ = 0.1f;                 // 移動速度
    MoveType moveType_;                      // 移動タイプ
    //====================================================
    // 弾発射関連
    //====================================================
    float bulletTimer_ = 0.0f;               // 経過時間（発射間隔測定）
    float bulletInterval_ = 5.0f;            // 発射間隔（秒）
    std::uniform_real_distribution<float> bulletIntervalDist_; // 発射間隔のランダム分布
    bool canShoot_ = true;                   // 弾を撃てる状態かどうか
    //====================================================
    // 死亡・状態管理
    //====================================================
    bool isDying_ = false;                   // 死亡エフェクト中かどうか
    float deathTimer_ = 0.0f;                // 死亡経過時間
    const float deathDuration_ = 1.0f;       // 死亡アニメーション時間（秒）
    bool isActive_ = false;                  // 有効かどうか
    bool isDead_ = false;                    // 完全に削除済みかどうか
public:   // アクセッサ（Getter / Setter）
	float GetSpawnBaseZ() const { return spawnBaseZ_; }
};