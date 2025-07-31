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

using namespace MatrixVector;

Player::~Player() {}

void Player::Initialize() {	 
    ModelManager::GetInstance()->LoadModel("Player.obj");     
    ModelManager::GetInstance()->LoadModel("Bullet/PlayerBullet.obj");
	TextureManager::GetInstance()->LoadTexture("uvChecker.png");

    // プレイヤーの初期位置と回転を設定
    transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f},  0.0f,0.0f,0.0f };
    // プレイヤー生成
    if (!object) {
        object = Object3d::Create("Player.obj", transform_);
    }
    targetpos_ = { {0.3f, 0.3f, 0.3f}, {0.0f, 0.0f, 0.0f}, {0.0f, 3.0f, 30.0f} };
    target_= Object3d::Create("Bullet/PlayerBullet.obj", targetpos_);
    moveDelta = Vector3(0.0f, 0.0f, 0.0f);

    reticleScreenPos = { 640.0f, 360.0f };
    targetreticle_ = Sprite::Create("uvChecker.png", reticleScreenPos, 0.0f, Vector2{ 200.0f,200.0f });
}

void Player::Update() {
    // カメラの現在位置取得
    Camera* camera = CameraManager::GetInstance()->GetActiveCamera();
    if (CameraManager::GetInstance()->GetcurrentMode() == CameraMode::GamePlay && camera) {
        Vector3 cameraPos = camera->GetTranslate();
        // カメラ相対オフセット位置にプレイヤーを固定
        Vector3 relativeOffset = { 0.0f, 0.0f, 3.0f };
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
    // デバッグ中のImGui表示
    DebugImgui();

    target_->SetTranslate(copypos);
    target_->Update();
    targetreticle_->Update();
}

void Player::Draw() {
    // プレイヤー描画
    object->Draw(); 
    target_->Draw();
}

void Player::DrawSprite() { 
   // targetreticle_->Draw();
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

    transform_.translate = moveDelta; // キー操作による移動量を加算
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
        bullet->Initialize(transform_.translate, copypos,1.5f);                     // 初期位置などを設定
		BulletManager::GetInstance()->AddPlayerBullet(std::move(bullet));                 // BulletManagerに追加
		canShoot_ = false;                                                          // 弾を撃てる状態にする
    };

    //if (Input::GetInstance()->Pushkey(DIK_SPACE)) {
    //    Matrix4x4 view = camera_->GetViewMatrix();
    //    Matrix4x4 proj = camera_->GetProjectionMatrix();

    //    Vector3 shootDirection = ScreenToWorldRay(reticleScreenPos, view, proj);

    //    std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
    //    bullet->Initialize(transform_.translate, transform_.translate + shootDirection * 10.0f, 1.5f);
    //    BulletManager::GetInstance()->AddPlayerBullet(std::move(bullet));
    //    canShoot_ = false;
    //}

}

Vector3 Player::ScreenToWorldRay(const Vector2& screenPos, const Matrix4x4& view, const Matrix4x4& projection) {
    // スクリーン座標 → NDC (-1 ～ +1)
    float ndcX = (2.0f * screenPos.x / 1280.0f) - 1.0f;
    float ndcY = 1.0f - (2.0f * screenPos.y / 720.0f); // Yは上下反転

    // NDC空間上のNear/Far位置
    Vector4 nearPoint = { ndcX, ndcY, 0.0f, 1.0f };
    Vector4 farPoint  = { ndcX, ndcY, 1.0f, 1.0f };

    // ビュー×プロジェクションの逆行列を計算
    Matrix4x4 viewProjInv = Inverse(Multiply(view, projection)); // ←順番注意: View→Proj→VP→逆行列

    // ワールド空間に変換
    Vector4 worldNear = TransformVector4(nearPoint, viewProjInv);
    Vector4 worldFar  = TransformVector4(farPoint, viewProjInv);

    // 同次座標を3Dに戻す
    worldNear.x /= worldNear.w;
    worldNear.y /= worldNear.w;
    worldNear.z /= worldNear.w;
    worldNear.w /= worldNear.w;

    worldFar.x /= worldNear.w;
    worldFar.y /= worldNear.w;
    worldFar.z /= worldNear.w;
    worldFar.w /= worldNear.w;
    
    
    // レイ方向 = Near → Far の正規化ベクトル
    Vector3 direction = Normalize(Vector3{
        worldFar.x - worldNear.x,
        worldFar.y - worldNear.y,
        worldFar.z - worldNear.z
    });

    return direction;
}

Vector4 Player::TransformVector4(const Vector4& vec, const Matrix4x4& mat) {
    Vector4 result;
    result.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + vec.w * mat.m[3][0];
    result.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + vec.w * mat.m[3][1];
    result.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + vec.w * mat.m[3][2];
    result.w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + vec.w * mat.m[3][3];
    return result;
}
