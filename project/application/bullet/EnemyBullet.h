#pragma once
#include <BaseBullet.h>
#include <BaseCharacter.h>

// ヘッダーの重複を防ぐ前方宣言
class Object3d;

/// <summary>
/// 敵の弾クラス
/// BaseBulletを継承
/// 敵弾の生成・移動・描画・衝突判定を管理する
/// </summary>
class EnemyBullet : public BaseBullet {
public:
    /// <summary>
    /// コンストラクタ
    /// メンバ変数の初期化などを行う
    /// </summary>
    EnemyBullet();
    /// <summary>
    /// デストラクタ
    /// リソースの解放処理を行う
    /// </summary>
    ~EnemyBullet() override;
    /// <summary>
    /// 弾の初期化（共通処理）
    /// モデルや基本設定のロードなどを行う
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 弾の初期化（発射位置・ターゲット位置を指定）
    /// </summary>
    /// <param name="startPos">発射位置（ワールド座標）</param>
    /// <param name="targetPos">狙うターゲットの位置（ワールド座標）</param>
    /// <param name="speed">弾の速度</param>
    void Initialize(const Vector3& startPos, const Vector3& targetPos, float speed) override;
    /// <summary>
    /// 終了処理
    /// メモリ解放などの後処理を行う
    /// </summary>
    void Finalize() override;
    /// <summary>
    /// 更新処理
    /// 移動・寿命管理・衝突判定などを行う
    /// </summary>
    void Update() override;
    /// <summary>
    /// 描画処理
    /// 弾モデルを3D空間に描画する
    /// </summary>
    void Draw() override;
    /// <summary>
    /// 弾の当たり判定情報（OBB）を取得する
    /// </summary>
    /// <returns>弾のOBB（回転付き当たり判定）</returns>
    OBB GetOBB() const;
private:    // メンバ変数
	// 3Dオブジェクト
    std::unique_ptr<Object3d> object_;
    // 座標
    Vector3 position_;
    // サイズ
    Vector3 size_;
	// 方向ベクトル
    Vector3 direction_;
	// 速度ベクトル
    Vector3 velocity_;
	// 現在の生存時間
    int time = 0;
	// 最大生存時間
    int Maxtime = 500;
public:  // アクセッサ（Getter / Setter）    
    /// 弾の現在位置を取得
    Vector3 GetPosition() const { return position_; }
    /// 弾のサイズを取得
    Vector3 GetRadius() const { return size_; }
    /// 弾の現在位置を設定
    void SetPosition(const Vector3& pos) { position_ = pos; } 
    /// 弾のサイズを設定
    void SetRadius(const Vector3& pos) { size_ = pos; }
};
