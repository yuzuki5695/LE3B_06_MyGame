#include "Player.h"
#include<ModelManager.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include<Input.h>
#include <CameraManager.h>
#include <MatrixVector.h>
#include<BulletManager.h>
#include<PlayerBullet.h>

using namespace MatrixVector;

Player::~Player() {}

void Player::Initialize() {
    ModelManager::GetInstance()->LoadModel("Player.obj");     
    ModelManager::GetInstance()->LoadModel("PlayerBullet.obj");
	// プレイヤーの初期位置と回転を設定
	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, -1.6f, 0.0f},  0.0f,3.0f,0.0f };
    // プレイヤー生成
    if (!object) {
        object = Object3d::Create("Player.obj", transform_);
    }
    targetpos_ = { {0.3f, 0.3f, 0.3f}, {0.0f, 0.0f, 0.0f}, {0.0f, 3.0f, 30.0f} };
    target_= Object3d::Create("PlayerBullet.obj", targetpos_);
    moveDelta = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Update() {
    // カメラの現在位置取得
    Camera* camera = CameraManager::GetInstance()->GetActiveCamera();
    if (camera) {
        Vector3 cameraPos = camera->GetTranslate();
        // カメラ相対オフセット位置にプレイヤーを固定
        Vector3 relativeOffset = { 0.0f, -3.0f, 30.0f };
        transform_.translate = cameraPos + relativeOffset;
    }
     
    UpdateBoostState(); // 追加：ブースト状態更新

    float currentSpeed = isBoosting_ ? boostSpeed_ : normalSpeed_;
    MoveInput(currentSpeed); // ブースト中は速く移動

    // ターゲットを矢印キーで動かす
    UpdateTargetPosition(targetpos_,0.2f);   // ターゲットに使う    
    // 弾の発射
    AttachBullet();

    // 移動後の位置をObjectに反映
    object->SetTranslate(transform_.translate);
    object->SetRotate(transform_.rotate);
    object->SetScale(transform_.scale);
    // プレイヤー更新
    object->Update();

    target_->SetTranslate(copypos);
    target_->Update();

	debugimgui(); // ImGuiでのデバッグ表示
}

void Player::Draw() {
    // プレイヤー描画
    object->Draw();
    target_->Draw();
}

void Player::debugimgui() {
#ifdef USE_IMGUI
    ImGui::Begin("Player Control");
    ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
    ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
    ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
    ImGui::End();
#endif // USE_IMGUI
}

void Player::MoveInput(float speed) {
    if (Input::GetInstance()->Pushkey(DIK_A)) moveDelta.x -= speed;
    if (Input::GetInstance()->Pushkey(DIK_D)) moveDelta.x += speed;
    if (Input::GetInstance()->Pushkey(DIK_W)) moveDelta.y += speed;
    if (Input::GetInstance()->Pushkey(DIK_S)) moveDelta.y -= speed;
    transform_.translate += moveDelta; // キー操作による移動量を加算
}

void Player::UpdateTargetPosition(Transform& targetTransform, float speed) {
    if (Input::GetInstance()->Pushkey(DIK_LEFT))  targetTransform.translate.x -= speed;
    if (Input::GetInstance()->Pushkey(DIK_RIGHT)) targetTransform.translate.x += speed;
    if (Input::GetInstance()->Pushkey(DIK_UP))    targetTransform.translate.y += speed;
    if (Input::GetInstance()->Pushkey(DIK_DOWN))  targetTransform.translate.y -= speed;
	copypos = targetpos_.translate + transform_.translate; // ターゲットの位置をプレイヤーの位置に合わせる
}

void Player::AttachBullet() { 
    bulletTimer_ += 1.0f / 60.0f; // 毎フレーム経過時間を加算（60fps前提）
    // 30秒経過したら発射可能にする
    if (bulletTimer_ >= bulletInterval_) {
        canShoot_ = true;
        bulletTimer_ = 0.0f; // タイマーリセット
    }
    // 弾が撃てるか確認
    if (!canShoot_) return;
    if (Input::GetInstance()->Pushkey(DIK_SPACE)) {                                 // スペースキーが押されたら弾を撃つ
        std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();		// 弾を生成
        bullet->Initialize(transform_.translate, copypos,1.5f);                     // 初期位置などを設定
		BulletManager::GetInstance()->AddPlayerBullet(std::move(bullet));                 // BulletManagerに追加
		canShoot_ = false;                                                          // 弾を撃てる状態にする
    };
}

void Player::UpdateBoostState() {
    Input* input = Input::GetInstance();

    // ブースト発動判定（Shiftキー押し && ブースト中でなく && クールタイム中でない）
    if (input->Pushkey(DIK_LSHIFT) && !isBoosting_ && !isCoolingDown_) {
        isBoosting_ = true;
        boostTime_ = 0.0f;
    }

    // ブースト中の処理
    if (isBoosting_) {
        boostTime_ += 1.0f / 60.0f; // 60FPS前提

        if (boostTime_ >= boostDuration_) {
            isBoosting_ = false;
            isCoolingDown_ = true;
            cooldownTime_ = 0.0f;
        }
    }

    // クールダウン中の処理
    if (isCoolingDown_) {
        cooldownTime_ += 1.0f / 60.0f;

        if (cooldownTime_ >= cooldownDuration_) {
            isCoolingDown_ = false;
        }
    }
}
