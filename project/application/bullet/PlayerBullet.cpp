#include "PlayerBullet.h"
#include <Object3d.h>
#include <ModelManager.h>
#include <MatrixVector.h>
#include<Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <CollisionConfig.h>

using namespace CollisionConfig;
using namespace LoadResourceID;
using namespace MatrixVector;

///====================================================
/// コンストラクタ
///====================================================
PlayerBullet::PlayerBullet() {}

///====================================================
/// デストラクタ
///====================================================
PlayerBullet::~PlayerBullet() {
    // 削除されるときにマネージャーから登録を解除する
    CollisionManager::GetInstance()->UnregisterCollider(this);
    Finalize();  // リソース解放
}

///====================================================
/// 終了処理（リソース解放）
///====================================================
void PlayerBullet::Finalize() {
	if (object_) {
		object_.reset(); // メモリ開放
	}
}

///====================================================
/// 初期化処理（共通）
/// ※今回は未使用、継承先や共通初期化用に残してある
///====================================================
void PlayerBullet::Initialize() {}
void PlayerBullet::Initialize(const Vector3& startPos, const Vector3& targetPos, float speed) {
    // 初期位置を設定
    transform_.translate = startPos;
    // 弾の大きさ（スケール）を設定  
    transform_.scale = { 0.5f, 0.5f, 0.5f };
    // モデルがまだ読み込まれていなければロード＆生成
    if (!object_) {
        // モデルを読み込む
        ModelManager::GetInstance()->LoadModel(model::Playerbullet);
        // 弾用の3Dオブジェクトを生成（Transform情報を渡す）
        object_ = Object3d::Create(
            model::Playerbullet,
            transform_
        );
        // スケール設定
        object_->SetScale({ 0.5f, 0.5f, 0.5f });
    }
    // 発射方向の計算：ターゲット方向ベクトルを正規化して向きを求める
    Vector3 dir = Normalize(targetPos - startPos);
    // 速度ベクトルを計算（方向 × 速度）
    velocity_ = dir * speed;
    // 弾をアクティブ状態にする（Update対象にする）
    active_ = true;
    // 弾の寿命管理用タイマー初期化
    time_ = 0;
    Maxtime_ = 1000;
}

///====================================================
/// 初期化処理（発射設定付き）
/// <param name="startPos">弾の発射位置</param>
/// <param name="targetPos">狙うターゲットの位置</param>
/// <param name="speed">弾の速度</param>
///====================================================
void PlayerBullet::Initialize(const Vector3& startPos, const Vector3& targetPos,const Vector3& cameraForward, float speed) {
    // 初期位置を設定
    transform_.translate = startPos;
    // 弾の大きさ（スケール）を設定  
    transform_.scale = { 0.5f, 0.5f, 0.5f };
    // モデルがまだ読み込まれていなければロード＆生成
    if (!object_) {
        // モデルを読み込む
        ModelManager::GetInstance()->LoadModel(model::Playerbullet);
        // 弾用の3Dオブジェクトを生成（Transform情報を渡す）
        object_ = Object3d::Create(model::Playerbullet, transform_);
        // スケール設定
        object_->SetScale({ 0.5f, 0.5f, 0.5f });
    }
      // --- 当たり判定の設定 ---
    this->SetCollisionAttribute(kGroupPlayerBullet); // 自身はプレイヤー弾
    this->SetCollisionMask(kGroupEnemy);           // 敵に当たりたい
    // 衝突マネージャーに登録
    CollisionManager::GetInstance()->RegisterCollider(this);
    // --- カメラ前方向に基づく修正 ---
    // 完全にターゲット方向へ飛ばす
    Vector3 dir = Normalize(targetPos - startPos);

    // ターゲット方向をカメラの前方向に「寄せる」
    float yaw = std::atan2(dir.x, dir.z);
    float pitch = -std::asin(dir.y);
    transform_.rotate = { pitch, yaw, 0.0f };
    // 速度ベクトルを計算（方向 × 速度）
    velocity_ = dir * speed;
    // 弾をアクティブ状態にする（Update対象にする）
    active_ = true;
    // 弾の寿命管理用タイマー初期化
    time_ = 0;
    Maxtime_ = 1000;
}

///====================================================
/// 更新処理
///====================================================
void PlayerBullet::Update() {
    // 速度ベクトルを加える
    transform_.translate = transform_.translate + velocity_;
    // 座標を更新
    object_->SetTranslate(transform_.translate);
    object_->SetRotate(transform_.rotate);
    object_->SetScale(transform_.scale);
    // 更新処理
    object_->Update();
    // 時間経過カウント（寿命制御）
    if (time_ < Maxtime_) {
        time_++;
    } else {
        // 一定時間経過後は非アクティブ化（削除対象）
        time_ = 0;
        active_ = false;
    }
}

///====================================================
/// 描画処理
///====================================================
void PlayerBullet::Draw() {
    object_->Draw();
}

// 共通ユーティリティを使ってOBBを取得
OBB PlayerBullet::GetOBB() const {
    return CollisionUtils::CreateOBB(transform_);
}

// 衝突時の処理
void PlayerBullet::OnCollision(Collider* other) {
    // 敵（またはターゲット）に当たったら自分を消す
    this->SetInactive();
}