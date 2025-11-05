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
#include <chrono>
#include <algorithm> // std::max を使用するために必要
#include <Skybox.h>
#include<ModelDate.h>

using namespace MatrixVector;

///=====================================================================
/// デストラクタ
///=====================================================================
Player::~Player() {}
///=====================================================================
/// 初期化処理
///=====================================================================
void Player::Initialize() {	 
    // モデル・テクスチャ読み込み
    ModelManager::GetInstance()->LoadModel("Gameplay/Model/Player/Player.obj");     
    ModelManager::GetInstance()->LoadModel("Bullet/PlayerBullet.obj");
    TextureManager::GetInstance()->LoadTexture("Target.png");

    // プレイヤーの初期位置と回転を設定
    transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f},  0.0f,0.0f,0.0f };
    // プレイヤー生成
    object = Object3d::Create("Gameplay/Model/Player/Player.obj", transform_);

    targetpos_ = { {0.3f, 0.3f, 0.3f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 30.0f} };
    target_= Object3d::Create("Bullet/PlayerBullet.obj", targetpos_);
    moveDelta = Vector3(0.0f, 0.0f, 0.0f);

    // レティクル初期化
    reticleScreenPos = { 640.0f, 360.0f }; // 画面中央 (仮に1280x720の場合)
    targetreticle_ = Sprite::Create("Target.png", reticleScreenPos, 0.0f, Vector2{ 100.0f, 100.0f });
    targetreticle_->SetTextureSize(Vector2{ 512.0f, 512.0f });
   	targetreticle_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準
    previousTime_ = 0.0f;

    // 死亡関連
    deathTimer_ = 0.0f;
    //// 回転速度（バラバラに回る感じ）
    deathRotateSpeed_ = { 0.05f, 0.07f, 0.02f };
    // 軽くスケールを上げる演出など
    transform_.scale = { 0.5f, 0.5f, 0.5f };
    fallVelocity = { 0.0f,0.0f,0.0f };
}
///=====================================================================
/// 更新処理
///=====================================================================
void Player::Update() {
    GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
    if (!gameCam) return;

    Vector3 bezierPos = gameCam->GetbezierPos();
    Vector3 relativeOffset = { 0.0f, -3.0f, 30.0f };

    // カメラの回転を取得（前方ベクトルから計算）
    Vector3 cameraForward = gameCam->GetForward(); // ← Getterを用意
    Vector3 cameraRight = Normalize(Cross({ 0,1,0 }, cameraForward));
    Vector3 cameraUp = Normalize(Cross(cameraForward, cameraRight));

    // カメラ座標系でオフセット変換
    Vector3 worldOffset =
        cameraRight * relativeOffset.x +
        cameraUp * relativeOffset.y +
        cameraForward * relativeOffset.z;

    // === プレイヤー位置更新 ===
    transform_.translate = bezierPos + worldOffset;

    // === プレイヤー向き更新（カメラと同じ方向：メイン時のみ）===
    Vector3 playerForward = cameraForward;
    // 現在のカメラモードを確認
    if (gameCam->GetMode() == ViewType::Main) {
        transform_.rotate = gameCam->GetActiveCamera()->GetRotate();
    }

    // 現在時刻を取得（秒）
    float currentTime = static_cast<float>(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

    // deltaTime を計算
    float deltaTime = currentTime - previousTime_;
    previousTime_ = currentTime;

            float currentSpeed = isBoosting_ ? boostSpeed_ : normalSpeed_;
        UpdateBoostState();
        MoveInput(currentSpeed); // ブースト中は速く移動 

    if (iskeyActive_) {

        // アクティブ中はキー操作を受け付ける
        if (isDeadEffectActive_ && active_ == false) {
            // プレイヤ―死亡演出
            StartDeathEffect();
        } else {
            // ターゲットを矢印キーで動かす
            UpdateTargetPosition(targetpos_, 0.2f);   // ターゲットに使う
            // 弾の発射
            AttachBullet();
        }
    }

    // デバッグ中のImGui表示
    DebugImgui();
    target_->SetTranslate(copypos);
    target_->Update();
    // 照準スプライトの位置更新（3D→2D変換)
    UpdateReticlePosition();
    targetreticle_->Update();

    // 移動後の位置をObjectに反映
    object->SetTranslate(transform_.translate);
    object->SetRotate(transform_.rotate);
    object->SetScale(transform_.scale);
    // プレイヤー更新
    object->Update();
}

///=====================================================================
/// 3D描画処理
///=====================================================================
void Player::Draw() {
    // プレイヤー描画
    object->Draw(); 
}
///=====================================================================
/// スプライト描画処理
///=====================================================================
void Player::DrawSprite() { 
    if (isReticleVisible_) {
        targetreticle_->Draw();
    }
}
///=====================================================================
/// デバッグ用ImGui
///=====================================================================
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
   GameCamera* cameramod = CameraManager::GetInstance()->GetGameCamera();
    // === 入力処理 ===
    Vector3 moveDelta = {0, 0, 0};
    Input* input = Input::GetInstance();
 //   if (cameramod->GetMode() == ViewType::Main && !isDeadEffectActive_) {
        if (input->Pushkey(DIK_A)) moveDelta.x -= speed;
        if (input->Pushkey(DIK_D)) moveDelta.x += speed;
        if (input->Pushkey(DIK_W)) moveDelta.y += speed;
        if (input->Pushkey(DIK_S)) moveDelta.y -= speed;
   // }

    // === 相対移動を制限（画面内の範囲）===
    // ここは「カメラから見たローカル座標」上での制限
    relativePos_.x = std::clamp(relativePos_.x + moveDelta.x, -10.0f, 10.0f);
    relativePos_.y = std::clamp(relativePos_.y + moveDelta.y, -2.5f, 9.0f);

    // === カメラ情報の取得 ===
    GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
    if (!gameCam) return;

    Vector3 cameraPos = gameCam->GetbezierPos();
    Vector3 forward = gameCam->GetForward();
    Vector3 right   = Normalize(Cross({0, 1, 0}, forward));
    Vector3 up      = Normalize(Cross(forward, right));

    // === カメラ基準の相対座標をワールド変換 ===
    Vector3 baseOffset = { 0.0f, -3.0f, 30.0f }; // カメラの前方に配置
    Vector3 totalOffset =
        right   * relativePos_.x +
        up      * relativePos_.y +
        forward * baseOffset.z +
        up      * baseOffset.y; // baseOffsetのYも考慮

    transform_.translate = cameraPos + totalOffset;

    // === 回転（カメラ方向に合わせる） メインカメラのみ===
    if (gameCam->GetMode() == ViewType::Main) {
        transform_.rotate = gameCam->GetMainCamera()->GetRotate();
    }
    // === モデル更新 ===
    object->SetTranslate(transform_.translate);
    object->SetRotate(transform_.rotate);
    object->Update();
}

///=====================================================================
/// ブースト状態更新処理
///=====================================================================
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
///=====================================================================
/// ターゲットの移動処理
///=====================================================================
void Player::UpdateTargetPosition(Transform& targetTransform, float speed) {
    // --- 入力でターゲットを動かす ---
    if (Input::GetInstance()->Pushkey(DIK_LEFT))  targetTransform.translate.x -= speed;
    if (Input::GetInstance()->Pushkey(DIK_RIGHT)) targetTransform.translate.x += speed;
    if (Input::GetInstance()->Pushkey(DIK_UP))    targetTransform.translate.y += speed;
    if (Input::GetInstance()->Pushkey(DIK_DOWN))  targetTransform.translate.y -= speed;

    // --- カメラ基準でターゲットを配置する ---
    GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
    if (!gameCam) return;

    Vector3 cameraPos = gameCam->GetbezierPos();
    Vector3 forward = gameCam->GetForward();
    Vector3 right   = Normalize(Cross({0, 1, 0}, forward));
    Vector3 up      = Normalize(Cross(forward, right));

    // === プレイヤーとの相対位置に応じたターゲット制限 ===
    // 画面内に相当する仮想座標範囲（例：X=-10～10, Y=-5～5 に対応させる）
    const float maxX = 12.0f;
    const float minX = -12.0f;
    const float maxY = 7.0f;
    const float minY = -7.0f;

    // --- 範囲内にクランプ（これで「画面外に出たら動かない」）---
    targetTransform.translate.x = std::clamp(targetTransform.translate.x, minX, maxX);
    targetTransform.translate.y = std::clamp(targetTransform.translate.y, minY, maxY);


    // 画面中央からの相対オフセット
    Vector3 offset =
        right * targetTransform.translate.x +
        up    * targetTransform.translate.y +
        forward * 30.0f; // カメラの前方方向に固定距離

    // プレイヤーの位置に加算
    copypos = transform_.translate + offset;
    target_->SetTranslate(copypos);
    target_->Update();
}

///=====================================================================
/// 弾の発射処理
///=====================================================================
void Player::AttachBullet() {
    bulletTimer_ += 1.0f / 60.0f;
    if (bulletTimer_ >= bulletInterval_) {
        canShoot_ = true;
        bulletTimer_ = 0.0f;
    }

    if (!canShoot_) return;
    if (Input::GetInstance()->Pushkey(DIK_SPACE)) {
        GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
        if (!gameCam) return;
        // カメラ情報
        Vector3 cameraForward = gameCam->GetForward();

        // 弾の初期位置（プレイヤーの少し前）
        Vector3 bulletStartPos = transform_.translate + gameCam->GetForward() * 2.0f;

        // --- ターゲット（レティクル3D）方向へ発射 ---
        Vector3 targetPos = target_->GetTranslate();
        Vector3 shootDir = Normalize(targetPos - bulletStartPos);

        std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
        bullet->Initialize(bulletStartPos, bulletStartPos + shootDir * 10.0f,cameraForward, 3.0f);


        BulletManager::GetInstance()->AddPlayerBullet(std::move(bullet));

        canShoot_ = false;
    }
}


///=====================================================================
/// レティクル用3D座標 → 2Dスクリーン座標変換
///=====================================================================
void Player::UpdateReticlePosition() {
    if (!target_ || !targetreticle_) return;

    // 3Dターゲット位置
    Vector3 targetWorldPos = target_->GetTranslate();

    // --- カメラから2Dスクリーン座標に変換 ---
    Camera* camera = CameraManager::GetInstance()->GetActiveCamera();
    const Matrix4x4& viewProj = camera->GetViewProjectionMatrix();

    Vector4 clipPos = {
        targetWorldPos.x * viewProj.m[0][0] + targetWorldPos.y * viewProj.m[1][0] + targetWorldPos.z * viewProj.m[2][0] + viewProj.m[3][0],
        targetWorldPos.x * viewProj.m[0][1] + targetWorldPos.y * viewProj.m[1][1] + targetWorldPos.z * viewProj.m[2][1] + viewProj.m[3][1],
        targetWorldPos.x * viewProj.m[0][2] + targetWorldPos.y * viewProj.m[1][2] + targetWorldPos.z * viewProj.m[2][2] + viewProj.m[3][2],
        targetWorldPos.x * viewProj.m[0][3] + targetWorldPos.y * viewProj.m[1][3] + targetWorldPos.z * viewProj.m[2][3] + viewProj.m[3][3]
    };

    if (clipPos.w != 0.0f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
    }

    const float screenWidth = 1280.0f;
    const float screenHeight = 720.0f;

    reticleScreenPos_.x = (clipPos.x * 0.5f + 0.5f) * screenWidth;
    reticleScreenPos_.y = (-clipPos.y * 0.5f + 0.5f) * screenHeight;

    targetreticle_->SetPosition(reticleScreenPos_);
}

///=====================================================================
/// OBB当たり判定取得
///=====================================================================
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

void Player::StartDeathEffect() {
    static float t = 0.0f;        // 時間経過
    const float gravity = 0.05f;
    const float gravitz = 0.01f;

    t += 1.0f / 60.0f;            // 60FPS換算
    fallVelocity.x = 0.0f;
    fallVelocity.y += gravity;
    fallVelocity.z += gravitz;

    // --- 徐々に加速する落下 ---
    transform_.translate.y -= fallVelocity.y;
    transform_.translate.z += fallVelocity.z;

    // --- 回転も時間で増加（イージング的）---
    transform_.rotate.x += 0.004f + 0.002f * sinf(t * 0.5f);
    transform_.rotate.z += 0.003f + 0.0015f * cosf(t * 0.4f);
}