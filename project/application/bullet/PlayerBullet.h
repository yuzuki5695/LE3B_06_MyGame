#pragma once
#include <BaseBullet.h>
#include <Transform.h>
#include <BaseCharacter.h>
#include<OBB.h>

// ヘッダーの重複を防ぐ前方宣言
class Object3d;

/// <summary>
/// プレイヤーの弾クラス
/// BaseBulletを継承
/// プレイヤーの弾の生成・移動・描画・衝突判定を管理する
/// </summary>
class PlayerBullet : public BaseBullet {
public: // メンバ関数
    /// <summary>
    /// コンストラクタ
    /// メンバ変数の初期化などを行う
    /// </summary>
    PlayerBullet();
    /// <summary>
    /// デストラクタ
    /// リソースの解放処理を行う
    /// </summary>
    ~PlayerBullet() override;
    /// <summary>
    /// 終了処理
    /// メモリ解放などの後処理を行う
    /// </summary>
    void Finalize() override;
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
    void Initialize(const Vector3& startPos, const Vector3& targetPos,float speed) override;

    void Initialize(const Vector3& startPos, const Vector3& targetPos, const Vector3& cameraForward, float speed);

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
private: // メンバ変数
	// 3Dオブジェクト
    std::unique_ptr<Object3d> object_;	   
    // 位置・回転・スケール情報 
    Transform transform_;
	// 方向ベクトル
    Vector3 direction_;
	// 速度ベクトル
    Vector3 velocity_;
	// 現在の生存時間
    int time_;
   	// 最大生存時間
    int Maxtime_;


public: // アクセッサ（Getter / Setter）
	/// Transformを取得
    Transform GetTransform() const { return transform_; }
    /// 現在位置を取得
    Vector3 GetTranslate() const { return transform_.translate ; }
    /// サイズを取得
    Vector3 GetScale() const { return transform_.scale; }


    /// Transformを設定
    void SetTranslate(const Transform& transform) {  transform_ = transform; } 
    /// 現在位置を設定
    void SetTranslate(const Vector3& translate) {  transform_.translate = translate; } 
    /// サイズを設定
    void SetScale(const Vector3& scale) { transform_.scale = scale; }
};