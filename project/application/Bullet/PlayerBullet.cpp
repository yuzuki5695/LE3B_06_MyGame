#include "PlayerBullet.h"
#include<Object3d.h>
#include "ModelManager.h"
#include <MatrixVector.h>

using namespace MatrixVector;

PlayerBullet::PlayerBullet(){}

PlayerBullet::~PlayerBullet() {	Finalize();}

void PlayerBullet::Initialize() {}

void PlayerBullet::Initialize(const Vector3& startPos, const Vector3& targetPos, float speed) {
	// 初期位置を設定
	position_ = startPos;
    if (!object_) {
        ModelManager::GetInstance()->LoadModel("uvChecker.obj");
        object_ = Object3d::Create(
            "uvChecker.obj",
            Transform{ { 0.5f, 0.5f, 0.5f},{ 0.0f, 0.0f, 0.0f},position_ }
        );
        object_->SetScale({ 0.5f, 0.5f, 0.5f });
    } 
    // 発射方向の初期化：ターゲットへのベクトル（正規化）
    Vector3 dir = Normalize(targetPos - startPos);
    velocity_ = dir * speed;
    active_ = true;
    hp_ = 1; // 初期HP
}

void PlayerBullet::Finalize() {
	if (object_) {
		object_.reset();
	}
}

void PlayerBullet::Update() {
    if (!active_) return;
    // 速度ベクトル velocity_ によって位置を移動
    position_ = position_ + velocity_;
    // オブジェクトの位置を更新
    object_->SetTranslate(position_);
    // 更新処理
    object_->Update();
    // 球が強制に消える距離を調整
    if (position_.z > 300.0f) {
        active_ = false;
    }
}

void PlayerBullet::Draw() {
	if (!active_) return;
	object_->Draw();
}

void PlayerBullet::Damage(int value) {
    hp_ -= value;
    if (hp_ <= 0) {
        hp_ = 0;
        active_ = false;  // HP0以下なら非アクティブにするなど
    }
}
