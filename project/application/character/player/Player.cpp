#include "Player.h"
#include <ModelManager.h>
#include <Input.h>
#include <PlayerState.h>
#include <CameraManager.h>
#include <PlayerDataLoader.h>
#include <TextureManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Models;
using namespace AssetGen::LoadResourceID::Textures;

namespace MyGame {

    Player::~Player() {}

    void Player::Initialize() {
        // 3Dオブジェクト生成
        TextureManager::GetInstance()->LoadTexture(Ui::Target);
        ModelManager::GetInstance()->LoadModel(Character::Player);

        data_ = PlayerDataLoader::Load("player");

        object_ = Object3d::Create(Character::Player, data_.transform);
        targettransform_ = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 30.0f} };

        target_ = Object3d::Create(Character::Player, targettransform_);

        // レティクル初期化
        targetreticle_ = Sprite::Create(Ui::Target, Vector2{ 640.0f, 360.0f }, 0.0f, Vector2{ 100.0f, 100.0f });
        targetreticle_->SetTextureSize(Vector2{ 512.0f, 512.0f });
        targetreticle_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準

        isActive_ = true;

        // コンポーネントの生成
        move_ = std::make_unique<PlayerMove>();
        reticle_ = std::make_unique<PlayerReticle>();
        attack_ = std::make_unique<PlayerAttack>();
        death_ = std::make_unique<PlayerDeath>();
        death_->Initialize();
        // 初期ステートをセットする
        ChangeState(std::make_unique<PlayerStateIdle>());
    }

    void Player::Update() {
        // ステートの更新
        state_.Update(*this);

        SyncWorldTransformByRail();

        targetreticle_->Update();
        target_->Update();
        object_->Update();
    }

    void Player::Draw() {
        if (object_) {
            object_->Draw();
        }
    }

    void Player::DrawSprite() {
        if (!isActive_) { return; }
        if (targetreticle_) {
            targetreticle_->Draw();
        }
    }

    void Player::SyncWorldTransformByRail() {
        GamePlayCamera* cam = CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>();
		Camera* active = CameraManager::GetInstance()->GetActiveCamera();
        
        Vector3 camPos = cam->GetBezierPos();
        Vector3 forward = cam->GetForward();
        Vector3 right = cam->GetRight();
        Vector3 up = cam->GetUp();

        // カメラ基準位置
        Vector3 baseOffset = { 0.0f, -3.0f, 30.0f };
        Vector3 baseWorld =
            right * baseOffset.x +
            up * baseOffset.y +
            forward * baseOffset.z;
        // 2. プレイヤーの入力移動分 (相対座標のXを右方向へ、Yを上方向へ適用)
        Vector3 moveOffset = (right * move_->GetRelativePos().x) + (up * move_->GetRelativePos().y);
        // 最終的なワールド座標
        Vector3 finalPos = camPos + baseWorld + moveOffset;
        object_->SetTranslate(finalPos);

        // 3. 回転の同期
        // カメラの回転をそのままコピーするのではなく、レールの向きに向かせる
        // または、旧コードのようにカメラの回転を反映させる
        object_->SetRotate(active->GetRotate()); // カメラの姿勢に合わせる
    }
}