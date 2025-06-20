#include "Bullet.h"
#include "ModelManager.h"

Bullet::Bullet() {}

Bullet::~Bullet() {
}

void Bullet::Initialize(const Vector3& position, const Vector3& direction, float speed) {
    position_ = position;
    velocity_ = direction * speed;

    if (!object_) {
        // 球モデルをロード済みと仮定。なければロード
        ModelManager::GetInstance()->LoadModel("uvChecker.obj");
        object_ = Object3d::Create("uvChecker.obj", Transform({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }));
    }

    object_->SetTranslate(position_);
    object_->SetScale({ 0.5f, 0.5f, 0.5f }); // 弾を少し小さく
    active_ = true;
}

void Bullet::Update() {
    if (!active_) return;

    // 位置を速度分移動
    position_ = position_ + velocity_;
 
    //Z方向だけ変更したいなら： 
    Vector3 pos = object_->GetTranslate();
    pos = position_;
    object_->SetTranslate(pos);

    //object_->SetTranslateZ(position_.z);
    
    // ここは必要に応じて調整
    if (position_.z > 300.0f) {
        active_ = false;
    }
    // 更新処理
    object_->Update();
}

void Bullet::Draw() {
    if (!active_) return;
    object_->Draw();
}