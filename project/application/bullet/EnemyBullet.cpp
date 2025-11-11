#include "EnemyBullet.h"
#include "ModelManager.h"
#include <MatrixVector.h>
#include <Object3d.h>

using namespace MatrixVector;

///====================================================
/// コンストラクタ
///====================================================
EnemyBullet::EnemyBullet(){}

///====================================================
/// デストラクタ）
///====================================================
EnemyBullet::~EnemyBullet() { Finalize(); }

///====================================================
/// 終了処理
///====================================================
void EnemyBullet::Finalize() {
    if (object_) {
        object_.reset(); // メモリ開放
    }
}

//====================================================
/// 初期化処理（共通）
/// ※今回は未使用、継承先や共通初期化用に残してある
///====================================================
void EnemyBullet::Initialize() {}

///====================================================
/// 初期化処理（発射設定付き）
/// <param name="startPos">弾の発射位置（ワールド座標）</param>
/// <param name="targetPos">弾が狙うターゲットの座標（ワールド座標）</param>
/// <param name="speed">弾の移動速度</param>
///====================================================
void EnemyBullet::Initialize(const Vector3& startPos, const Vector3& targetPos, float speed) {
    // 弾の初期位置を設定
    transform_.translate = startPos;
    // 弾の大きさ（スケール）を設定
    transform_.scale = { 0.5f,0.5f,0.5f };
    // モデル未生成の場合はロード・生成する
    if (!object_) {
        // モデルを読み込む
        ModelManager::GetInstance()->LoadModel("EnemyBullet.obj");
        // 弾用の3Dオブジェクトを生成
        object_ = Object3d::Create(
            "EnemyBullet.obj",
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
    Maxtime_ = 500;
}
///====================================================
/// 初期化処理（発射設定付き）
/// <param name="startPos">弾の発射位置</param>
/// <param name="targetPos">狙うターゲットの位置</param>
/// <param name="speed">弾の速度</param>
///====================================================
void EnemyBullet::Initialize(const Vector3& startPos, const Vector3& targetPos, const Vector3& cameraForward, float speed) {
    // 初期位置を設定
    transform_.translate = startPos;
    // 弾の大きさ（スケール）を設定  
    transform_.scale = { 0.5f, 0.5f, 0.5f };
    // モデルがまだ読み込まれていなければロード＆生成
    if (!object_) {
        // モデルを読み込む
        ModelManager::GetInstance()->LoadModel("Bullet/EnemyBullet.obj");
        // 弾用の3Dオブジェクトを生成（Transform情報を渡す）
        object_ = Object3d::Create("Bullet/EnemyBullet.obj", transform_);
        // スケール設定
        object_->SetScale({ 0.5f, 0.5f, 0.5f });
    }

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
void EnemyBullet::Update() {
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
void EnemyBullet::Draw() {       
	object_->Draw();
}

///====================================================
/// 当たり判定用OBBの取得
///====================================================
OBB EnemyBullet::GetOBB() const {
    OBB obb;

    // 中心は現在の位置
    obb.center = transform_.translate;

    // サイズ（スケール）の半分をハーフサイズにする
    obb.halfSize = { transform_.scale.x };

    // 軸はXYZの単位ベクトル
    obb.axis[0] = {1, 0, 0}; // X軸
    obb.axis[1] = {0, 1, 0}; // Y軸
    obb.axis[2] = {0, 0, 1}; // Z軸

    return obb;
}