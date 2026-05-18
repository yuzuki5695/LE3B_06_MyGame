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
#include <EditorEntityRegistry.h>
#include <EditorTypes.h>
#include <CollisionConfig.h>
#include <Enemy.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>


using namespace MyEngine;
using namespace MatrixVector;
using namespace AssetGen::LoadResourceID::Models;
using namespace AssetGen::LoadResourceID::Textures;

namespace MyGame {
     
    using namespace CollisionConfig;

    Player::~Player() {}

    void Player::Finalize() {
        collider_.reset(); // コライダーの破棄
        object_.reset();   // 3Dオブジェクトの破棄
    }

    void Player::Initialize() {
        // テクスチャの読み込み
        TextureManager::GetInstance()->LoadTexture(Ui::Target);
        // モデルの読み込み
        ModelManager::GetInstance()->LoadModel(Character::Player);
        // データの読み込み
        data_ = PlayerDataLoader::Load("player");
        // カメラ位置からのオフセットを設定
        baseOffset_ = { 0.0f, -3.0f, 30.0f };
        // プレイヤーオブジェクトの生成、初期化
        object_ = Object3d::Create(Character::Player, data_.transform);

        // 状態フラグの初期化
        flags_.isAlive = true;
        flags_.isActive = true;

        // コライダー生成
        collider_ = Collider::Create({ .profile = Profile::Player,.obb = CollisionUtils::CreateOBB(object_.get()) });
        // 衝突時の処理
        collider_->SetCallback([this](Collider* other) {
            if (!IsAlive()) { return; }
                ChangeState(std::make_unique<PlayerStateDead>());            
            });
        // コライダー登録
        CollisionManager::GetInstance()->RegisterCollider(collider_.get());

        // ターゲットオブジェクトの生成、初期化
        targettransform_ = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 30.0f} };
        target_ = Object3d::Create(Character::Player, targettransform_);

        // レティクル初期化
        targetreticle_ = Sprite::Create(Ui::Target, Vector2{ 640.0f, 360.0f }, 0.0f, Vector2{ 100.0f, 100.0f });
        targetreticle_->SetTextureSize(Vector2{ 512.0f, 512.0f });
        targetreticle_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準

        // コンポーネントの生成
        move_ = std::make_unique<PlayerMove>();          // 移動ロジックの生成
        reticle_ = std::make_unique<PlayerReticle>();    // レティクルロジックの生成
        attack_ = std::make_unique<PlayerAttack>();      // 攻撃ロジックの生成
        death_ = std::make_unique<PlayerDeath>();        // 死亡演出ロジックの生成
        death_->Initialize();     // 死亡演出の初期化
        // 初期ステートをセットする
        ChangeState(std::make_unique<PlayerStateIdle>());
        // ImGuiの登録
        DrawImGui();
    }

    void Player::Update() {
        // ステートの更新
        state_.Update(*this);

		// カメラがGamePlayCameraで更新中の場合、プレイヤーのワールド座標をカメラ位置に基づいて更新する
        if (CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()) {
            // 死亡状態ではないなら
            if (!dynamic_cast<PlayerStateDead*>(state_.GetCurrentState())) {
                SyncWorldTransformByRail();
            }
        }

        // 各コンポーネントの更新
        targetreticle_->Update();
        target_->Update();
        object_->Update();
    }

    void Player::Draw() {
		// 描画処理
        object_->Draw();
    }

    void Player::DrawSprite() {
		// 活動していない場合は描画処理をスキップ
        if (!IsActive()) { return; }
        if (targetreticle_) {
            targetreticle_->Draw();
        }
    }

    void Player::SyncWorldTransformByRail() {
		// カメラ位置を基準にプレイヤーのワールド座標を更新する
        GamePlayCamera* cam = CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>();
        Camera* active = CameraManager::GetInstance()->GetActiveCamera();
		// カメラのワールド座標を取得
        Vector3 camPos = active->GetTranslate();

        // カメラ位置から固定オフセット
        Vector3 offset = { move_->GetRelativePos().x,move_->GetRelativePos().y + baseOffset_.y,baseOffset_.z };
		// カメラ回転を考慮したオフセットの回転
        Vector3 finalPos = camPos + offset;
		// プレイヤーオブジェクトの座標を更新
        object_->SetTranslate(finalPos);

        //Vector3 camRot = active->GetRotate();
        //Vector3 finalRot{};
        //// カメラ回転を基準
        //finalRot.x = camRot.x + move_->GetTiltX();
        //finalRot.y = camRot.y;
        //finalRot.z = camRot.z + move_->GetTiltZ() + move_->GetExtraRotationZ();

        //object_->SetRotate(finalRot);
    }

    void Player::DrawImGui() {
#ifdef USE_IMGUI
        EditorTypes::EditorObjectInfo info;
        info.name = "Player";                                 // エディタの登録されるオブジェクト名
        info.category = EditorTypes::ObjectCategory::Object3D;  // 登録するオブジェクトのカテゴリ
        info.objectPtr = object_.get();                       // 扱うオブジェクトのポインタ
        info.drawEditor = [this]() {                            // パラメータの情報を登録
            /// ======================================
            /// (engine側の基本のパラメータ) 
            /// ======================================
            if (object_) {
                object_->DrawImGui("object3d");
            }
            };
        // オブジェクト情報を登録する
        EditorEntityRegistry::Instance().Register(info);
#endif // USE_IMGUI    
    }
}