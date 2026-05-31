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
//#include <ParticleManager.h>
//#include <ParticleEmitter.h>
#include <LineRenderer.h>
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
        // 当たり判定サイズ
        colliderSize_ = object_->GetScale();
        // コライダー生成
        collider_ = Collider::Create({ .profile = Profile::Player,.obb = CollisionUtils::CreateOBB(object_.get(),colliderSize_) });
        // ラインのサイズをコライダーに合わせる
        LineRenderer::GetInstance()->SetSize(colliderSize_);
        // 衝突時の処理
        collider_->SetCallback([this](Collider* other) {
#ifdef USE_IMGUI
            // Debug用
            LineRenderer::GetInstance()->SetHit(true);
#endif // USE_IMGUI
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

        if (collider_ && object_) {
            collider_->SetOBB(CollisionUtils::CreateOBB(object_.get(), colliderSize_));
        }
        // 各コンポーネントの更新
        targetreticle_->Update();
        target_->Update();
        object_->Update();
#ifdef USE_IMGUI
        // LineRendererクラスにある基本のパラメータ
        const auto& debug = LineRenderer::GetInstance()->GetDebugSettings();
        // デバッグ表示フラグで表示、非表示
        if (debug.enable && object_) {
            LineRenderer::GetInstance()->SetHit(false);
            // 衝突状態で色変更
            Vector4 hitColor = debug.isHit ? Vector4{ 1,0,0,1 } : Vector4{ 0,1,0,1 };
            // 現在のObjectからOBB生成
            OBB obb = CollisionUtils::CreateOBB(object_.get(), colliderSize_);
            // ライン描画
            LineRenderer::GetInstance()->AddOBB(obb, hitColor);
        }
#endif // USE_IMGUI
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
        // カメラを取得
        Camera* active = CameraManager::GetInstance()->GetActiveCamera();
        if (!active) { return; }
        // カメラ位置
        Vector3 camPos = active->GetTranslate();
        // カメラ回転
        Vector3 camRot = active->GetRotate();
        // カメラ基準ベクトル
        float yaw = camRot.y;
        float pitch = camRot.x;
        // カメラの回転から前方向ベクトルを計算
        Vector3 forward = { sinf(yaw) * cosf(pitch),-sinf(pitch),cosf(yaw) * cosf(pitch) };
        // ベクトルの正規化
        forward = Normalize(forward);
        // ワールド上の上方向
        Vector3 worldUp = { 0,1,0 };
        Vector3 right = Normalize(Cross(worldUp, forward));
        Vector3 up = Normalize(Cross(forward, right));
        // プレイヤーローカル座標        
        Vector3 relative = { move_->GetRelativePos().x,move_->GetRelativePos().y + baseOffset_.y,baseOffset_.z };
        // カメラ向きへ変換
        Vector3 finalPos = camPos + right * relative.x + up * relative.y + forward * relative.z;
        // プレイヤーオブジェクトの座標を更新
        object_->SetTranslate(finalPos);
    }

    void Player::GainExp(uint32_t exp) {
        exp_ += exp;
        CheckLevelUp();
    }

    void Player::CheckLevelUp() {
        // 経験値が規定値を超えている間、レベルアップを繰り返す
        while (exp_ >= nextLevelExp_) {
            exp_ -= nextLevelExp_;
            level_++;
            // 次のレベルへの必要経験値を増加させる
            nextLevelExp_ = static_cast<uint32_t>(nextLevelExp_ * 1.5f);
            // レベルアップ演出（エフェクトや音）をここに初期化しても良い
        }
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
            /// ======================================
            /// Player Status
            /// ======================================
            ImGui::SeparatorText("Player Status");
            // レベル
            ImGui::Text("Level : %u", level_);
            // 現在経験値
            ImGui::Text("EXP : %u / %u", exp_, nextLevelExp_);
            // 経験値バー
            float expRate = 0.0f;

            if (nextLevelExp_ > 0) {
                expRate = static_cast<float>(exp_) / static_cast<float>(nextLevelExp_);
            }
            ImGui::ProgressBar(expRate, ImVec2(200.0f, 20.0f));
            // デバッグ用
            if (ImGui::Button("Add EXP +10")) { GainExp(10); }
            // LineRenderer基本パラメータ
            LineRenderer::GetInstance()->DrawImGui(colliderSize_);
            };
        // オブジェクト情報を登録する
        EditorEntityRegistry::Instance().Register(info);
#endif // USE_IMGUI    
    }
}