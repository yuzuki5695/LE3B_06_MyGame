#include "Player.h"
#include <ModelManager.h>
#include <CameraManager.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#include <Input.h>
#include<PlayerBullet.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include<TextureManager.h>
#include <algorithm>

using namespace MatrixVector;

Player::~Player() {}

void Player::Initialize() {	 
    ModelManager::GetInstance()->LoadModel("Player.obj");     
    ModelManager::GetInstance()->LoadModel("Bullet/PlayerBullet.obj");
    TextureManager::GetInstance()->LoadTexture("Target.png");

    // プレイヤーの初期位置と回転を設定
    transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f},  0.0f,0.0f,0.0f };
    // プレイヤー生成
    object = Object3d::Create("Player.obj", transform_);

    targetpos_ = { {0.3f, 0.3f, 0.3f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 30.0f} };
    target_= Object3d::Create("Bullet/PlayerBullet.obj", targetpos_);
    moveDelta = Vector3(0.0f, 0.0f, 0.0f);

    // レティクル初期化
    reticleScreenPos = { 640.0f, 360.0f }; // 画面中央 (仮に1280x720の場合)
    targetreticle_ = Sprite::Create("Target.png", reticleScreenPos, 0.0f, Vector2{ 100.0f, 100.0f });
    targetreticle_->SetTextureSize(Vector2{ 512.0f, 512.0f });
}

void Player::Update() {
    GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
    if (gameCam) {
        Vector3 bezierPos = gameCam->GetbezierPos(); // ← Getter が必要
        Vector3 relativeOffset = { 0.0f, -3.0f, 30.0f }; // カメラの前方に出すなど
        transform_.translate = bezierPos + relativeOffset;
    }

    UpdateBoostState(); // 追加：ブースト状態更新

    float currentSpeed = isBoosting_ ? boostSpeed_ : normalSpeed_;
    MoveInput(currentSpeed); // ブースト中は速く移動

    // ターゲットを矢印キーで動かす
    UpdateTargetPosition(targetpos_,0.2f);   // ターゲットに使う
    // 弾の発射
    AttachBullet();

    // デバッグ中のImGui表示
    DebugImgui();
    target_->SetTranslate(copypos);
    target_->Update();

    //UpdateReticlePosition();   // 追加：3D空間に合わせてスクリーン位置を更新
    //targetreticle_->SetPosition(reticleScreenPos_);  // スプライト位置を更新
    targetreticle_->Update();
 
    // 移動後の位置をObjectに反映
    object->SetTranslate(transform_.translate);
    object->SetRotate(transform_.rotate);
    object->SetScale(transform_.scale);
    // プレイヤー更新
    object->Update();
}

void Player::Draw() {
    // プレイヤー描画
    object->Draw(); 
    target_->Draw();
}

void Player::DrawSprite() { 
    targetreticle_->Draw();
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

    // --- 入力による移動量を制限 ---
    moveDelta.x = std::clamp(moveDelta.x, -10.0f, 10.0f);
    moveDelta.y = std::clamp(moveDelta.y, -5.0f, 8.0f);

    // 回転角度の処理
    float targetZ = 0.0f;
    float tiltAmount = isBoosting_ ? 6.0f : 0.3f;

    if (Input::GetInstance()->Pushkey(DIK_A)) targetZ = tiltAmount;
    else if (Input::GetInstance()->Pushkey(DIK_D)) targetZ = -tiltAmount;

    transform_.rotate.z += (targetZ - transform_.rotate.z) * 0.1f;

    // --- プレイヤーの位置を更新 ---
    transform_.translate += moveDelta;

    // --- 最終的な位置の制限 ---
    transform_.translate.x = std::clamp(transform_.translate.x, -10.0f, 10.0f);
    transform_.translate.y = std::clamp(transform_.translate.y, -5.0f, 5.0f);
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
        bullet->Initialize(transform_.translate, copypos, 3.0f);                     // 初期位置などを設定
        BulletManager::GetInstance()->AddPlayerBullet(std::move(bullet));                 // BulletManagerに追加
        canShoot_ = false;                                                          // 弾を撃てる状態にする
    };
}

void Player::UpdateReticlePosition() {
    Camera* camera = CameraManager::GetInstance()->GetActiveCamera();
    if (!camera) return;

    Matrix4x4 view = camera->GetViewMatrix();
    Matrix4x4 projection = camera->GetProjectionMatrix();
    Matrix4x4 vp = Multiply(projection, view); // ViewProjection行列

    Vector3 worldPos = copypos; // ターゲット3D位置

    Vector4 pos4 = { worldPos.x, worldPos.y, worldPos.z, 1.0f };
    Vector4 clipPos = MultiplyM4xV4(vp, pos4);

    if (clipPos.w != 0.0f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
        clipPos.z /= clipPos.w;
    }

    // スクリーン座標に変換
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;

    reticleScreenPos_.x = (clipPos.x * 0.5f + 0.5f) * screenWidth;
    reticleScreenPos_.y = (-clipPos.y * 0.5f + 0.5f) * screenHeight;
}

OBB Player::GetOBB() const {
    OBB obb;

    // 中心座標
    obb.center = transform_.translate;

    // ハーフサイズ（スケールの半分）
    obb.halfSize = {
        transform_.scale.x / 2.0f, 
        transform_.scale.y / 2.0f, 
        transform_.scale.z / 2.0f
    };

    // 回転行列（XYZ順で回転を合成）
    Matrix4x4 rotX = MakeRotateXMatrix(transform_.rotate.x);
    Matrix4x4 rotY = MakeRotateYMatrix(transform_.rotate.y);
    Matrix4x4 rotZ = MakeRotateZMatrix(transform_.rotate.z);

    // Z→X→Y の順に掛けることで、XYZ回転（ローカル空間回転）を再現
    Matrix4x4 rotMat = Multiply(Multiply(rotZ, rotX), rotY);

    // ローカル軸（X, Y, Z）をワールド空間へ回転適用
    obb.axis[0] = Normalize(Multiply4x4x3(rotMat, Vector3{ 1, 0, 0 })); // X軸
    obb.axis[1] = Normalize(Multiply4x4x3(rotMat, Vector3{ 0, 1, 0 })); // Y軸
    obb.axis[2] = Normalize(Multiply4x4x3(rotMat, Vector3{ 0, 0, 1 })); // Z軸

    return obb;
}