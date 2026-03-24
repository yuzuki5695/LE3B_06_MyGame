#include "Player.h"
#include <ModelManager.h>
#include <CameraManager.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI
#include <Input.h>
#include <PlayerBullet.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include <TextureManager.h>
#include <algorithm>
#include <chrono>
#include <Skybox.h>
#include <ModelDate.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <PlayerDataLoader.h>

using namespace LoadResourceID;
using namespace MatrixVector;

Player::~Player() {}
Vector3 Player::GetForward() {
    return {
        sinf(transform_.rotate.y),
        0.0f,
        cosf(transform_.rotate.y)
    };
}
void Player::Initialize() {
    // モデル・テクスチャ読み込み
    ModelManager::GetInstance()->LoadModel(model::Player);
    ModelManager::GetInstance()->LoadModel(model::Playerbullet);
    TextureManager::GetInstance()->LoadTexture(texture::Target);

    data_ = PlayerDataLoader::Load("player");
    transform_ = data_.transform;
    // プレイヤー生成
    object = Object3d::Create(model::Player, transform_);

    targettransform_ = { {0.3f, 0.3f, 0.3f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 30.0f} };
    target_ = Object3d::Create(model::Playerbullet, targettransform_);

    // レティクル初期化
    targetreticle_ = Sprite::Create(texture::Target, Vector2{ 640.0f, 360.0f }, 0.0f, Vector2{ 100.0f, 100.0f });
    targetreticle_->SetTextureSize(Vector2{ 512.0f, 512.0f });
    targetreticle_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準

    // 軽くスケールを上げる演出など
    transform_.scale = { 0.5f, 0.5f, 0.5f };

    // インスタンスの生成
    move_ = std::make_unique<PlayerMove>();
    move_->Initialize(data_.move);
    reticle_ = std::make_unique<PlayerReticle>();
    reticle_->Initialize(data_.reticle);
    weapon_ = std::make_unique<PlayerWeapon>();
    weapon_->Initialize(data_.weapon);
    death_ = std::make_unique<PlayerDeath>();
    death_->Initialize(data_.death);
    activeState_ = std::make_unique<PlayerStateAlive>();
}

void Player::Update() {
    CameraManager* camMgr = CameraManager::GetInstance();
    Camera* activeCam = camMgr->GetMainCamera();

    // GamePlayCameraのみ更新処理を行う
    if (camMgr->GetActiveSceneCamera() == SceneCameraType::Gameplay) {
        SyncWorldTransformByRail(); // ワールド座標同期処理
        // メインカメラ中
        if (CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main) {
            // プレイヤー回転もカメラに合わせる
            transform_.rotate = activeCam->GetRotate();
        }
        
        // 状態固有の更新
        if (activeState_) {
            activeState_->Update(this);
        }

        //// 現在の状態に応じて関数を呼び分け
        //switch (state_) {
        //case State::Alive:   UpdateAlive();   break;
        //case State::Dead:    UpdateDead();    break;
        //}

        // デバッグ中のImGui表示
        DebugImgui();
        targetreticle_->Update();
     
        // ターゲット（3D照準）に反映
        target_->SetTranslate(targettransform_.translate);
        target_->Update();
        // Objectに反映
        object->SetTranslate(transform_.translate);
        object->SetRotate(transform_.rotate);
        object->SetScale(transform_.scale);
    }
    // プレイヤー更新(GamePlayCamera以外ではただのobjectとして扱う)
    object->Update();
}

void Player::Draw() {
    // プレイヤー描画
    object->Draw(); 
}

void Player::DrawSprite() { 
    if (state_ == State::Alive) {
        targetreticle_->Draw();
    }
}

void Player::DebugImgui() {
#ifdef USE_IMGUI

    //object->DrawImGui("Player");
    //ImGui::Begin("Player Control");
    //ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
    //ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
    //ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
    //ImGui::End();
#endif // USE_IMGUI
}

void Player::UpdateAlive() {
 //   CameraManager* camMgr = CameraManager::GetInstance();
	//// 移動処理の更新（ローカル座標での移動量を取得）
 //   move_->Update(transform_,camMgr->GetMainCamera()->GetRotate());
 //   // レティクル
 //   reticle_->Update(targettransform_, transform_.translate, target_.get());
 //   // 攻撃
 //   weapon_->Update(transform_.translate, target_->GetTranslate(), camMgr->GetGameplayCamera());
 //   // レティクル(2Dスプライト)の同期
 //   reticle_->UpdateSprite(target_->GetTranslate(), targetreticle_.get(), camMgr->GetActiveCamera());
}

void Player::UpdateDead() {
    //// 死亡演出：ここに落下や回転のロジックを書く（後にクラス化も可能）
    //death_->Update(transform_.rotate, deathOffset_, object.get());
    //// 座標の同期（これが重要）
    //SyncWorldTransformByRail();
}


void Player::SyncWorldTransformByRail() {
    CameraManager* camMgr = CameraManager::GetInstance();
    GamePlayCamera* gameCam = camMgr->GetGameplayCamera();
    Camera* activeCam = camMgr->GetMainCamera();

    // カメラのレール情報
    Vector3 camPos = gameCam->GetBezierPos();
    Vector3 forward = gameCam->GetForward();
    Vector3 right = gameCam->GetRight();
    Vector3 up = gameCam->GetUp();
    // 1. 基本となるオフセット位置（カメラの30m前方、3m下）
    Vector3 cameraBaseOffset = { 0.0f, -3.0f, 30.0f };
    Vector3 cameraBaseWorld = (right * cameraBaseOffset.x) + (up * cameraBaseOffset.y) + (forward * cameraBaseOffset.z);

    // 2. 自機の移動分（move_から取得）をワールド方向に変換して加算
    Vector3 playerMoveOffset = (right * move_->GetLocalPosition().x) + (up * move_->GetLocalPosition().y);
    Vector3 basePos = camPos + cameraBaseWorld + playerMoveOffset;
    // 3. 回転の同期（メインカメラ時）
    if (camMgr->GetTypeview() == ViewCameraType::Main) {
        transform_.rotate = activeCam->GetRotate();
    }

    // 4. 座標の最終決定（死亡演出中のSubカメラならオフセットを加算）
    if (camMgr->GetTypeview() == ViewCameraType::Sub) {
        transform_.translate = basePos + deathOffset_;
    } else {
        transform_.translate = basePos;
    }
}