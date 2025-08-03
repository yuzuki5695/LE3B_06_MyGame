#include "EnemyBullet.h"
#include "ModelManager.h"
#include <MatrixVector.h>
#include <Object3d.h>

using namespace MatrixVector;

EnemyBullet::EnemyBullet(){}

EnemyBullet::~EnemyBullet() { Finalize(); }

void EnemyBullet::Initialize() {}

void EnemyBullet::Initialize(const Vector3& startPos, const Vector3& targetPos, float speed) {
	// 初期位置を設定
	position_ = startPos;
    size_ = { 0.5f,0.5f,0.5f };
    if (!object_) {
        ModelManager::GetInstance()->LoadModel("EnemyBullet.obj");
        object_ = Object3d::Create(
            "EnemyBullet.obj",
            Transform{ size_,{ 0.0f, 0.0f, 0.0f},position_ }
        );
        object_->SetScale({ 0.5f, 0.5f, 0.5f });
    } 
    // 発射方向の初期化：ターゲットへのベクトル（正規化）
    Vector3 dir = Normalize(targetPos - startPos);
    velocity_ = dir * speed;
    active_ = true;
}

void EnemyBullet::Finalize() {
	if (object_) {
		object_.reset();
	}
}

void EnemyBullet::Update() {

    // 速度ベクトル velocity_ によって位置を移動
    position_ = position_ + velocity_;
    // オブジェクトの位置を更新
    object_->SetTranslate(position_);
    // 更新処理
    object_->Update();
        if (time < Maxtime) {
        time++;
    } else {
        time = 0; 
        active_ = false;
    }
}

void EnemyBullet::Draw() {       
	object_->Draw();
}