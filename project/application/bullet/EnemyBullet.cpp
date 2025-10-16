#include "EnemyBullet.h"
#include "ModelManager.h"
#include <MatrixVector.h>
#include <Object3d.h>

using namespace MatrixVector;

EnemyBullet::EnemyBullet(){}

///====================================================
/// デストラクタ
/// オブジェクト破棄時に呼ばれる（Finalizeを通してリソース解放）
///====================================================
EnemyBullet::~EnemyBullet() { Finalize(); }

///====================================================
/// 終了処理
/// モデルや3Dオブジェクトなどのリソースを解放する
///====================================================
void EnemyBullet::Finalize() {
    if (object_) {
        object_.reset(); // メモリ開放（unique_ptrをnullptrにする）
    }
}

//====================================================
/// 初期化処理（共通）
/// ※今回は未使用だが、継承先や共通初期化用に残してある
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
    position_ = startPos;
    // 弾の大きさ（スケール）を設定
    size_ = { 0.5f,0.5f,0.5f };

    // モデル未生成の場合はロード・生成する
    if (!object_) {
        // モデルを読み込む
        ModelManager::GetInstance()->LoadModel("EnemyBullet.obj");
        // 弾用の3Dオブジェクトを生成（Transform情報を渡す）
        object_ = Object3d::Create(
            "EnemyBullet.obj",
            Transform{ size_,{ 0.0f, 0.0f, 0.0f},position_ }
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
}

///====================================================
/// 更新処理
/// ・位置更新（velocity_で移動）
/// ・寿命（time）管理
/// ・3Dオブジェクトの更新
///====================================================
void EnemyBullet::Update() {
    // 弾を移動させる
    position_ = position_ + velocity_;

    // 座標を更新
    object_->SetTranslate(position_);

    // 更新処理
    object_->Update();

    // 時間経過カウント（寿命制御）
    if (time < Maxtime) {
        time++;
    } else {
        // 一定時間経過後は非アクティブ化（削除対象）
        time = 0;
        active_ = false;
    }
}

///====================================================
/// 描画処理
/// モデルを3D空間に描画
///====================================================
void EnemyBullet::Draw() {       
	object_->Draw();
}

///====================================================
/// 当たり判定用OBBの取得
/// <returns>現在の位置・スケールに基づいたOBB</returns>
///====================================================
OBB EnemyBullet::GetOBB() const {
    OBB obb;

    // 中心は現在の位置
    obb.center = position_;

    // サイズ（スケール）の半分をハーフサイズにする
    obb.halfSize = {
        size_.x / 2.0f,
        size_.y / 2.0f,
        size_.z / 2.0f
    };

    // プレイヤーバレットは基本的に回転しないと想定して軸はXYZの単位ベクトルでOK
    obb.axis[0] = Vector3{1, 0, 0}; // X軸
    obb.axis[1] = Vector3{0, 1, 0}; // Y軸
    obb.axis[2] = Vector3{0, 0, 1}; // Z軸

    return obb;
}