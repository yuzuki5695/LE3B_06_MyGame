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
#include <LineRenderer.h>
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
		CollisionManager::GetInstance()->UnregisterCollider(collider_.get()); // コライダーの登録解除
    }
    ///====================================================
    /// 初期化処理
    ///====================================================
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

        kMaxLevel = 3;      // 最大レベル
        level_ = 1;         // 現在のレベル
        exp_ = 0;           // 現在の経験値
        nextLevelExp_ = 60; // 次のレベルに必要な経験値
        isStateUpdateEnabled_ = true;
        isLevelUpRequested_ = false;
        maxHp_ = 5; // 必要に応じて調整、またはPlayerData等からロード
        hp_ = maxHp_;  // 初期状態は満タン     
        isInvincible_ = false;       // 無敵フラグ
        invincibleTimer_ = 0.0f;    // 無敵残り時間タイマー
		kInvincibleTime = 3.0f; // 無敵時間の長さ
        isVisible_ = true; // 点滅状態の可視性フラグ
        blinkTimer_ = 0.0f; // 点滅タイマー
        kBlinkInterval_ = 0.1f; // 点滅間隔

        // 当たり判定サイズ
        collidersize_ = object_->GetScale();
        // コライダー生成
        collider_ = Collider::Create({ .profile = Profile::Player,.obb = CollisionUtils::CreateOBB(object_.get(),collidersize_) });
        // ラインのサイズをコライダーに合わせる
        LineRenderer::GetInstance()->SetSize(collidersize_);
        // 衝突時の処理
        collider_->SetCallback([this](Collider* other) {
#ifdef USE_IMGUI
            // Debug用
            LineRenderer::GetInstance()->SetHit(true);
#endif // USE_IMGUI
            if (!IsAlive()) { return; }
            // 無敵状態ならダメージ処理そのものをスキップ
            if (isInvincible_) { return; }
            ApplyDamage(1); // 仮のダメージ値

            });
        // コライダー登録
        CollisionManager::GetInstance()->RegisterCollider(collider_.get());

        // ターゲットオブジェクトの生成、初期化
        target_ = Object3d::Create(Character::Player, { {0.1f, 0.1f, 0.1f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} });

        // レティクル初期化
        targetreticle_ = Sprite::Create(Ui::Target, Vector2{ 640.0f, 360.0f }, 0.0f, Vector2{ 100.0f, 100.0f });
        targetreticle_->SetTextureSize(Vector2{ 512.0f, 512.0f });
        targetreticle_->SetAnchorPoint(Vector2{ 0.5f, 0.5f }); // 中心基準

        // コンポーネントの生成
        move_ = std::make_unique<PlayerMove>();          // 移動ロジックの生成
        reticle_ = std::make_unique<PlayerReticle>();    // レティクルロジックの生成
        attack_ = std::make_unique<PlayerAttack>();      // 攻撃ロジックの生成
        death_ = std::make_unique<PlayerDeath>();        // 死亡演出ロジックの生成
        // 各処理の初期化
        death_->Initialize();
        move_->Initialize();
        // 初期ステートをセットする
        ChangeState(std::make_unique<PlayerStateIdle>());
        // ImGuiの登録
        DrawImGui();
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void Player::Update() {
        float deltaTime = 1.0f / 60.0f;
        // 無敵タイマーの更新処理
        if (isInvincible_) {
            // 無敵時間のカウントダウン
            invincibleTimer_ -= deltaTime;    
            // 点滅タイマー
            blinkTimer_ += deltaTime;
            // 点滅処理
            if (blinkTimer_ >= kBlinkInterval_) {
				// 点滅間隔を超えたら可視状態を切り替える
                blinkTimer_ = 0.0f;
                isVisible_ = !isVisible_;
            }
			// 点滅処理
            if (invincibleTimer_ <= 0.0f) {
                invincibleTimer_ = 0.0f;
                isInvincible_ = false;
                // 無敵終了
                isVisible_ = true;
                blinkTimer_ = 0.0f;
            }
            object_->SetMaterialColor({ 1.0f, 0.0f, 0.0f, 1.0f });
        }
        else {
            // 無敵状態でない場合は常に可視状態にする
            isVisible_ = true;            
            object_->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        }

		// イベントロックされていない場合のみ更新処理を行う
        if (!IsEventLocked()) {
            // ステートの更新
            if (isStateUpdateEnabled_) {
                state_.Update(*this);
            }
			// コライダーのOBBをプレイヤーオブジェクトの現在の状態に合わせて更新
            if (collider_ && object_ && IsActive()) {
                collider_->SetOBB(CollisionUtils::CreateOBB(object_.get(), collidersize_));
            }
        }
        
        // カメラがGamePlayCameraで更新中の場合、プレイヤーのワールド座標をカメラ位置に基づいて更新する
        if (CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()) {
            // 死亡状態ではないなら
            if (!dynamic_cast<PlayerStateDead*>(state_.GetCurrentState())) {
                SyncWorldTransformByRail();
            }
        }

        // 各オブジェクト更新
        targetreticle_->Update();
        target_->Update();
        object_->Update();
#ifdef USE_IMGUI
        if (IsActive()) {
			// ライン表示はGamePlayCameraのときのみ
            if (CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()) {
                // LineRendererクラスにある基本のパラメータ
                const auto& debug = LineRenderer::GetInstance()->GetDebugSettings();
                // デバッグ表示フラグで表示、非表示
                if (debug.enable && object_) {
                    LineRenderer::GetInstance()->SetHit(false);
                    // 衝突状態で色変更
                    Vector4 hitColor = debug.isHit ? Vector4{ 1,0,0,1 } : Vector4{ 0,1,0,1 };
                    // ライン描画
                    LineRenderer::GetInstance()->AddOBB(collider_->GetOBB(), hitColor);
                }
            }
        }
#endif // USE_IMGUI
    }
    ///====================================================
    /// 描画処理(3D)
    ///====================================================
    void Player::Draw() {
        if (!isVisible_) {
            return;
        }
        // 描画処理
        object_->Draw();
    }
    ///====================================================
    /// 描画処理(2D)
    ///====================================================
    void Player::DrawSprite() {
        // 活動していない場合は描画処理をスキップ
        if (!IsActive()) { return; }
        if (targetreticle_) {
            targetreticle_->Draw();
        }
    }
    
    void Player::ApplyDamage(uint32_t damage) {
        // すでに死亡している、または無敵状態なら処理しない
        if (hp_ <= 0 || isInvincible_) return;
		// ダメージを適用
        hp_ -= damage;
		// HPが0未満にならないように制限
        if (hp_ < 0) {
            hp_ = 0;
        }
        // ダメージを受けたら無敵状態にする
        if (hp_ > 0) {
            isInvincible_ = true;
            invincibleTimer_ = kInvincibleTime; // 3秒セット
        }
        // HPが0になったら死亡状態へ遷移する処理
        if (hp_ == 0) {            
            ChangeState(std::make_unique<PlayerStateDead>());
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
        Vector3 playerPos = camPos + right * relative.x + up * relative.y + forward * relative.z;
        // プレイヤーオブジェクトの座標を更新
        object_->SetTranslate(playerPos);
        // Targetをプレイヤー前方に配置
        constexpr float kTargetDistance = 30.0f;
        Vector3 targetPos = playerPos + forward * kTargetDistance + right * reticle_->GetOffset().x + up * reticle_->GetOffset().y;
        target_->SetTranslate(targetPos);
        Vector2 screenPos = reticle_->WorldToScreen(target_->GetTranslate(), active);
        targetreticle_->SetPosition(screenPos);
    }
    
    Vector3 Player::GetExpTargetPosition() const {
        if (!object_) { return {}; }
		// プレイヤーのワールド座標を取得
        Vector3 pos = object_->GetTranslate();
		// カメラが存在しない場合はそのまま返す
        if (!CameraManager::GetInstance()->GetActiveCamera()) {
            return pos;
        }
		// カメラの回転を取得
        Vector3 camRot = CameraManager::GetInstance()->GetActiveCamera()->GetRotate();
        float yaw = camRot.y;
        float pitch = camRot.x;
        Vector3 forward = { sinf(yaw) * cosf(pitch), -sinf(pitch), cosf(yaw) * cosf(pitch) };
        forward = Normalize(forward);
        // 少し上
        pos.y += 1.5f;
        // プレイヤー前方へ寄せる
        pos += forward * 2.0f;
        return pos;
    }

    void Player::GainExp(uint32_t exp) {
		// 経験値を加算
        exp_ += exp;
		// レベルアップの条件をチェック
        CheckLevelUp();
    }

    void Player::CheckLevelUp() {
        // 最大レベルなら経験値加算停止
        if (level_ >= kMaxLevel) {
            level_ = kMaxLevel;
            exp_ = 0;
            return;
        }
        // 経験値が規定値を超えている間
        while (exp_ >= nextLevelExp_ && level_ < kMaxLevel) {
            exp_ -= nextLevelExp_;
            level_++;

            // レベルアップ通知
            isLevelUpRequested_ = true;

            // 最大レベルになったら打ち切り
            if (level_ >= kMaxLevel) {
                level_ = kMaxLevel;
                exp_ = 0;
                break;
            }
            // 次レベル必要経験値
            nextLevelExp_ = static_cast<uint32_t>(nextLevelExp_ * 1.2f);
        }
    }

    Vector3 Player::GetForward() const {
		// カメラの回転から前方向ベクトルを計算
        Camera* active = CameraManager::GetInstance()->GetActiveCamera();
        Vector3 rot = active->GetRotate();
        float yaw = rot.y;
        float pitch = rot.x;
		// 前方向ベクトルを計算
        Vector3 forward = { sinf(yaw) * cosf(pitch), -sinf(pitch), cosf(yaw) * cosf(pitch) };
		// 正規化して返す
        return Normalize(forward);
    }

    bool Player::ConsumeLevelUpRequest() {
		// レベルアップ要求があるかを返し、フラグをリセットする
        bool result = isLevelUpRequested_;
        isLevelUpRequested_ = false;
        return result;
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
            if (level_ >= 3) {
                ImGui::Text("Level : MAX");
                ImGui::ProgressBar(1.0f, ImVec2(200.0f, 20.0f), "MAX");
            } else {
                ImGui::Text("Level : %u", level_);         // レベル
                ImGui::Text("EXP : %u / %u", exp_, nextLevelExp_);    // 現在経験値
                float expRate = static_cast<float>(exp_) / static_cast<float>(nextLevelExp_);
                ImGui::ProgressBar(expRate, ImVec2(200.0f, 20.0f)
                );
                // デバッグ用
                if (ImGui::Button("Add EXP +10")) { GainExp(10); }
            }
            /// --------------------------------------
            /// ★追加：HP ＆ 無敵時間のデバッグ表示
            /// --------------------------------------
            ImGui::Separator(); // 区切り線
            ImGui::Text("--- HP & Invincible ---");
            
            // HP表示 (バー形式)
            ImGui::Text("HP : %u / %u", hp_, maxHp_);
            float hpRate = (maxHp_ > 0) ? static_cast<float>(hp_) / static_cast<float>(maxHp_) : 0.0f;
            ImGui::ProgressBar(hpRate, ImVec2(200.0f, 20.0f), hp_ == 0 ? "DEAD" : nullptr);

            // 無敵状態の表示
            if (isInvincible_) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Status: INVINCIBLE");
                ImGui::Text("Invincible Timer: %.2f sec", invincibleTimer_);
            } else {
                ImGui::Text("Status: Normal");
            }

            // デバッグ用手動ダメージボタン
            if (ImGui::Button("Apply 1 Damage")) {
                ApplyDamage(1);
            }
			// デバッグ用手動回復ボタン
            ImGui::SameLine();
            if (ImGui::Button("Heal Full")) {
                hp_ = maxHp_;
                isInvincible_ = false;
                invincibleTimer_ = 0.0f;
            } 

            // LineRenderer基本パラメータ
            LineRenderer::GetInstance()->DrawImGui(collidersize_);
            };
        // オブジェクト情報を登録する
        EditorEntityRegistry::Instance().Register(info);
#endif // USE_IMGUI    
    }
}