#include "Player.h"
#include <ModelManager.h>
#include <Input.h>
#include <PlayerState.h>
#include <CameraManager.h>
#include <PlayerDataLoader.h>
#include <TextureManager.h>
#include <MatrixVector.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>
#include <EditorEntityRegistry.h>
#include <EditorTypes.h>

using namespace MyEngine;
using namespace MatrixVector;
using namespace AssetGen::LoadResourceID::Models;
using namespace AssetGen::LoadResourceID::Textures;

namespace MyGame {

    Player::~Player() {}

    void Player::Initialize() {
        // 3Dオブジェクト生成
        TextureManager::GetInstance()->LoadTexture(Ui::Target);
        ModelManager::GetInstance()->LoadModel(Character::Player);

        data_ = PlayerDataLoader::Load("player");

        baseOffset_ = { 0.0f, -3.0f, 30.0f };

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

#ifdef USE_IMGUI

        Editor::EditorObjectInfo info;
        info.name = "Player";
        info.category = Editor::EditorObjectCategory::Object3D;
        info.objectPtr = this;

        info.drawFunc = [](void* ptr) {
            Player* player = static_cast<Player*>(ptr);
            if (!player) {
                return;
            }

            // =========================
            // Object3d の ImGui
            // =========================

            if (player->object_) {
                player->object_->DrawImGui("Player");
            }

            // =========================
            // Player 固有
            // =========================

            if (ImGui::TreeNode("Player")) {

                ImGui::DragFloat3(
                    "Base Offset",
                    &player->baseOffset_.x,
                    0.1f
                );

  /*              ImGui::Checkbox(
                    "Active",
                    &player->isActive_
                );*/

                ImGui::TreePop();
            }


            };

        info.aliveFunc = [](void* ptr) {return ptr != nullptr;};
        EditorEntityRegistry::Instance().Register(info);

#endif // USE_IMGUI
    }

    void Player::Update() {
        // ステートの更新
        state_.Update(*this);

        if (CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()) {
            SyncWorldTransformByRail();
        }

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

        Vector3 camPos = active->GetTranslate();

        // カメラ位置から固定オフセット
        Vector3 offset = { move_->GetRelativePos().x,move_->GetRelativePos().y + baseOffset_.y,baseOffset_.z };

        Vector3 finalPos = camPos + offset;

        object_->SetTranslate(finalPos);

        //Vector3 camRot = active->GetRotate();
        //Vector3 finalRot{};
        //// カメラ回転を基準
        //finalRot.x = camRot.x + move_->GetTiltX();
        //finalRot.y = camRot.y;
        //finalRot.z = camRot.z + move_->GetTiltZ() + move_->GetExtraRotationZ();

        //object_->SetRotate(finalRot);
    }
}