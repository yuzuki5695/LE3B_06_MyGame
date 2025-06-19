#include "Player.h"
#include<ModelManager.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include<Input.h>
#include <CameraManager.h>
#include <MatrixVector.h>

using namespace MatrixVector;

Player::~Player() { 
    for (auto b : bullets_) {
        delete b;
    }
    bullets_.clear();
}

void Player::Initialize() {

    ModelManager::GetInstance()->LoadModel("uvChecker.obj");
    
	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, -1.6f, 0.0f}, {0.0f, 3.0f, 0.0f} };

    // プレイヤー生成
    object = Object3d::Create("uvChecker.obj", transform); 
    
    
    targetpos_ = { {0.3f, 0.3f, 0.3f}, {0.0f, -1.6f, 0.0f}, {0.0f, 3.0f, 30.0f} };
    target_= Object3d::Create("uvChecker.obj", targetpos_);

    moveDelta = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Update() {
    if (isActive_) {
        // キー入力でmoveDeltaに1フレーム分の移動量加算
        if (Input::GetInstance()->Pushkey(DIK_A)) moveDelta.x -= 0.1f;
        if (Input::GetInstance()->Pushkey(DIK_D)) moveDelta.x += 0.1f;
        if (Input::GetInstance()->Pushkey(DIK_W)) moveDelta.y += 0.1f;
        if (Input::GetInstance()->Pushkey(DIK_S)) moveDelta.y -= 0.1f;

        // ターゲットを矢印キーで動かす
        if (Input::GetInstance()->Pushkey(DIK_LEFT))  targetpos_.translate.x -= 0.3f;
        if (Input::GetInstance()->Pushkey(DIK_RIGHT)) targetpos_.translate.x += 0.3f;
        if (Input::GetInstance()->Pushkey(DIK_UP))    targetpos_.translate.y += 0.3f;
        if (Input::GetInstance()->Pushkey(DIK_DOWN))  targetpos_.translate.y -= 0.3f;

        // 弾の発射
        attachBullet();

        // カメラの現在位置取得
        Camera* camera = CameraManager::GetInstance()->GetActiveCamera();
        if (camera) {
            Vector3 cameraPos = camera->GetTranslate();
            // カメラ相対オフセット位置にプレイヤーを固定
            Vector3 relativeOffset = { 0.0f, -3.0f, 30.0f };
            transform.translate = cameraPos + relativeOffset;
        }

        transform.translate += moveDelta; // キー操作による移動量を加算

        copypos = targetpos_.translate + transform.translate;

        // 移動後の位置をObjectに反映
        object->SetTranslate(transform.translate);
        object->SetRotate(transform.rotate);
        object->SetScale(transform.scale);
        // プレイヤー更新
        object->Update();

        target_->SetTranslate(copypos);
        target_->Update();
    }
    // 弾の更新
    for (auto& b : bullets_) {
        if (b->IsActive()) {
            b->Update();
        }
    }
#ifdef USE_IMGUI
    ImGui::Begin("Player Control");
    ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
    ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
    ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);
    ImGui::End();
#endif // USE_IMGUI
}

void Player::Draw() {
    // プレイヤー描画
    if (isActive_) {
        object->Draw();
        target_->Draw();
    }
    // 弾の描画
    for (auto& b : bullets_) {
        if (b->IsActive()) {
            b->Draw();
        }
    }
}

void Player::attachBullet() {
    bulletTimer_ += 1.0f / 60.0f; // 毎フレーム経過時間を加算（60fps前提）
    // 30秒経過したら発射可能にする
    if (bulletTimer_ >= bulletInterval_) {
        canShoot_ = true;
        bulletTimer_ = 0.0f; // タイマーリセット
    }

    // 弾が撃てるか確認
    if (!canShoot_) return;

    // スペースキーが押されたら弾を撃つ
    if (Input::GetInstance()->Pushkey(DIK_SPACE)) {
        Bullet* newBullet = new Bullet();
        // 発射位置：プレイヤーの現在位置
        Vector3 startPos = transform.translate;
        // ワールド座標のターゲット位置を取得
        Vector3 targetWorldPos = copypos;
        // 発射方向：ターゲットへのベクトル（正規化）
        Vector3 direction = Normalize(targetWorldPos - startPos);

        newBullet->Initialize(startPos, direction, 0.5f);
        bullets_.push_back(newBullet);

        canShoot_ = false;
    }


}