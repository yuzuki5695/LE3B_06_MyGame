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

    ModelManager::GetInstance()->LoadModel("uvChecker.obj");
	// プレイヤーの初期位置と回転を設定
	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, -1.6f, 0.0f}, {0.0f, 3.0f, 0.0f} };
    // プレイヤー生成
    object = Object3d::Create("uvChecker.obj", transform); 
     
    targetpos_ = { {0.3f, 0.3f, 0.3f}, {0.0f, -1.6f, 0.0f}, {0.0f, 3.0f, 30.0f} };
    target_= Object3d::Create("uvChecker.obj", targetpos_);
    moveDelta = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Update() {
    // カメラの現在位置取得
    Camera* camera = CameraManager::GetInstance()->GetActiveCamera();
    if (camera) {
        Vector3 cameraPos = camera->GetTranslate();
        // カメラ相対オフセット位置にプレイヤーを固定
        Vector3 relativeOffset = { 0.0f, -3.0f, 30.0f };
        transform.translate = cameraPos + relativeOffset;
    }
    
    // キー入力でmoveDeltaに移動量加算
    MoveInput(0.1f);
    // ターゲットを矢印キーで動かす
    UpdateTargetPosition(targetpos_,0.2f);   // ターゲットに使う    
    // 弾の発射
    AttachBullet();

    // 移動後の位置をObjectに反映
    object->SetTranslate(transform.translate);
    object->SetRotate(transform.rotate);
    object->SetScale(transform.scale);
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
    ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
    ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
    ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);
    ImGui::End();
#endif // USE_IMGUI
}

void Player::MoveInput(float speed) {
    if (Input::GetInstance()->Pushkey(DIK_A)) moveDelta.x -= speed;
    if (Input::GetInstance()->Pushkey(DIK_D)) moveDelta.x += speed;
    if (Input::GetInstance()->Pushkey(DIK_W)) moveDelta.y += speed;
    if (Input::GetInstance()->Pushkey(DIK_S)) moveDelta.y -= speed;
    transform.translate += moveDelta; // キー操作による移動量を加算
}

void Player::UpdateTargetPosition(Transform& targetTransform, float speed) {
    if (Input::GetInstance()->Pushkey(DIK_LEFT))  targetTransform.translate.x -= speed;
    if (Input::GetInstance()->Pushkey(DIK_RIGHT)) targetTransform.translate.x += speed;
    if (Input::GetInstance()->Pushkey(DIK_UP))    targetTransform.translate.y += speed;
    if (Input::GetInstance()->Pushkey(DIK_DOWN))  targetTransform.translate.y -= speed;
	copypos = targetpos_.translate + transform.translate; // ターゲットの位置をプレイヤーの位置に合わせる
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
        std::unique_ptr<BaseBullet> bullet = std::make_unique<PlayerBullet>();		// 弾を生成
        bullet->Initialize(transform.translate, copypos, 0.5f);                     // 初期位置などを設定
		BulletManager::GetInstance()->AddBullet(std::move(bullet));                 // BulletManagerに追加
		canShoot_ = false;                                                          // 弾を撃てる状態にする
    };
}