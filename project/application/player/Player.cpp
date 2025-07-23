#include "Player.h"
#include <ModelManager.h>
#include <CameraManager.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include <Input.h>

Player::~Player() {}

void Player::Initialize() {	 
    ModelManager::GetInstance()->LoadModel("Player.obj");
	// プレイヤーの初期位置と回転を設定
	//transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, -1.6f, 0.0f},  0.0f,3.0f,0.0f };
    // 
    // プレイヤー生成
    if (!object) {
        object = Object3d::Create("Player.obj", transform_);
    }

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

    
    // 移動後の位置をObjectに反映
    object->SetTranslate(transform_.translate);
    object->SetRotate(transform_.rotate);
    object->SetScale(transform_.scale);
    // プレイヤー更新
    object->Update(); 
    // デバッグ中のImGui表示
    DebugImgui();
}

void Player::Draw() {
    // プレイヤー描画
    object->Draw();
}

void Player::DebugImgui() {
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

    
    // 回転の目標角度
    float targetZ = 0.0f;

    // 回転角度の強さ（ブースト中は大きめに傾ける）
    float tiltAmount = isBoosting_ ? 6.0f : 0.3f;

    if (Input::GetInstance()->Pushkey(DIK_A)) targetZ = tiltAmount;
    else if (Input::GetInstance()->Pushkey(DIK_D)) targetZ = -tiltAmount;

    // 緩やかに回転角を補間（Lerp）
    transform_.rotate.z += (targetZ - transform_.rotate.z) * 0.1f;

    transform_.translate += moveDelta; // キー操作による移動量を加算
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