#include "GamePlayUI.h"
#include <TextureManager.h>
#include <Input.h>
#include <CameraManager.h>
#include <Easing.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Textures;
using namespace Easing;
using namespace MatrixVector;

namespace MyGame {
    void GamePlayUI::Initialize() {
        // 操作UIテクスチャ一覧
        const std::array<const char*, 22> operationTextures = {
            Operationui::W,Operationui::A,Operationui::S,Operationui::D,
            Operationui::ArrowUp,Operationui::ArrowLeft,Operationui::ArrowDown,
            Operationui::ArrowRight,Operationui::SPACEKey,Operationui::SHIFT,Operationui::W_RED,
            Operationui::A_RED,Operationui::S_RED,Operationui::D_RED,Operationui::ArrowUp_RED,
            Operationui::ArrowLeft_RED,Operationui::ArrowDown_RED,Operationui::ArrowRight_RED,Operationui::SPACEKey_RED,
            Operationui::SHIFT_RED,Ui::Gage,Ui::Player_ui
        };

        // 操作UIをまとめて読み込み
        for (const auto& texture : operationTextures) {
            TextureManager::GetInstance()->LoadTexture(texture);
        }

        // ステージ進行度UIの生成
        gage_ = Sprite::Create(Ui::Gage, Vector2{ 380.0f, 10.0f }, 0.0f, Vector2{ 500.0f,30.0f });
        // プレイヤー位置UIの生成
        player_ui_ = Sprite::Create(Ui::Player_ui, Vector2{ 380.0f, 12.3f }, 0.0f, Vector2{ 25.0f,25.0f });

        // 操作UIの生成
        Vector2 size = { 40.0f, 40.0f };
        Vector2 center = { 85.0f, 470.0f };
        CreateWASDUI(center, size, 4.0f, 25.0f);

        //  操作UIの初期サイズを保存
        for (auto& ui : uis_) {
            uiOriginalSizes_.push_back(ui->GetSize());
            ui->SetAnchorPoint({ 0.5f, 0.5f });
            ui->SetSize({ 0.0f, 0.0f });
        }

        isAnimating_ = true; // アニメーション開始フラグ
        timer_ = 0.0f; // アニメーション開始からの経過時間
        duration_ = 0.5f; // 0.5秒でアニメーションが完了するように設定

        // ポーズメニューの初期化、生成
        pausemenu_ = std::make_unique<Pausemenu>();
        pausemenu_->Initialize();

        // プレイヤーに合わせて表示するUI
        expFollowUI_ = Sprite::Create(Operationui::W, Vector2{ 110.0f, 360.0f }, 0.0f, Vector2{ 48.0f,48.0f });
        expFollowUI_->SetAnchorPoint({ 0.5f,0.5f });

        isEventLocked = true;
    }

    void GamePlayUI::Update() {
        // ポーズ画面の更新
        pausemenu_->Update();
        // ステージ進行度UIの更新
        UpdateStageProgressUI();
        // 操作UIのアニメーション更新
        UpdateControlUIAnimation();
        // 操作UIの更新
        UpdateControlUI();        
        // プレイヤー位置UIの更新
        UpdatePlayerFollowUI();
        gage_->Update();
        player_ui_->Update();
        expFollowUI_->Update();
        for (std::unique_ptr<Sprite>& ui : uis_) {
            ui->Update();
        }
    }

    void GamePlayUI::Draw() {
		// ポーズ画面の描画
        pausemenu_->Draw();
		// ステージ進行度UIの描画
        gage_->Draw();
		// プレイヤー位置UIの描画
        player_ui_->Draw();
		// プレイヤーに合わせて表示するUIの描画
        //expFollowUI_->Draw();
        // 操作UIの描画
        for (std::unique_ptr<Sprite>& ui : uis_) {
            ui->Draw();
        }
    }

    void GamePlayUI::UpdatePlayerFollowUI() {
        if (!player_) { return; }

        Camera* camera = CameraManager::GetInstance()->GetActiveCamera();

        if (!camera) { return; }

        // プレイヤー頭上
        Vector3 worldPos = player_->GetTranslate();
        worldPos.y += 3.0f;

        // ViewProjection
        Matrix4x4 view = camera->GetViewMatrix();
        Matrix4x4 projection = camera->GetProjectionMatrix();
        Matrix4x4 viewProjection = Multiply(view, projection);
        // ワールド→クリップ空間
        Vector3 clipPos = TransformPoint(worldPos, viewProjection);
        // 背面なら非表示
        if (clipPos.z <= 0.0f) {
            expFollowUI_->SetColor({ 1,1,1,0 });
            return;
        }

        expFollowUI_->SetColor({ 1,1,1,1 });
        constexpr float screenWidth = 1280.0f;
        constexpr float screenHeight = 720.0f;
        // NDC → Screen
        Vector2 screenPos;
        screenPos.x = (clipPos.x + 1.0f) * 0.5f * screenWidth;
        screenPos.y = (1.0f - clipPos.y) * 0.5f * screenHeight;
        expFollowUI_->SetPosition(screenPos);
    }
            
    void GamePlayUI::UpdateStageProgressUI() {
        // マネージャ経由で「今のカメラ挙動」から進捗を直接もらう
        float progress = CameraManager::GetInstance()->GetCurrentBehaviorAs<GamePlayCamera>()->GetProgress();
        progress = std::clamp(progress, 0.0f, 1.0f);
        constexpr float gageX = 380.0f;
        constexpr float gageWidth = 500.0f;
        constexpr float playerWidth = 25.0f;

        float movableWidth = gageWidth - playerWidth;
        float uiX = gageX + progress * movableWidth;
        player_ui_->SetPosition(Vector2{ uiX, 12.3f });
    }

    void GamePlayUI::CreateWASDUI(const Vector2& baseCenter, const Vector2& size, float keySpacing, float groupSpacing) {
        uis_.clear();

        float groupHeight = size.y * 2 + keySpacing;
        // W段 + ASD段の高さ

        // =====================
        // 1. WASD グループ
        // =====================
        Vector2 wasdCenter = baseCenter;

        Vector2 wPos = { wasdCenter.x, wasdCenter.y };
        Vector2 sRowY = { 0.0f,wasdCenter.y + size.y + keySpacing };

        Vector2 aPos = { wasdCenter.x - size.x - keySpacing, sRowY.y };
        Vector2 sPos = { wasdCenter.x, sRowY.y };
        Vector2 dPos = { wasdCenter.x + size.x + keySpacing, sRowY.y };

        uis_.push_back(Sprite::Create(Operationui::W, wPos, 0.0f, size)); //0
        uis_.push_back(Sprite::Create(Operationui::A, aPos, 0.0f, size)); //1
        uis_.push_back(Sprite::Create(Operationui::S, sPos, 0.0f, size)); //2
        uis_.push_back(Sprite::Create(Operationui::D, dPos, 0.0f, size)); //3


        // =====================
        // 2. Arrow グループ
        // =====================
        float arrowTopY = wasdCenter.y + groupHeight + groupSpacing;

        Vector2 arrowTop = { baseCenter.x, arrowTopY };
        float arrowRowY = arrowTopY + size.y + keySpacing;

        Vector2 upPos = { arrowTop.x, arrowTop.y };
        Vector2 leftPos = { arrowTop.x - size.x - keySpacing, arrowRowY };
        Vector2 downPos = { arrowTop.x, arrowRowY };
        Vector2 rightPos = { arrowTop.x + size.x + keySpacing, arrowRowY };

        uis_.push_back(Sprite::Create(Operationui::ArrowUp, upPos, 0.0f, size));     //4
        uis_.push_back(Sprite::Create(Operationui::ArrowRight, leftPos, 0.0f, size)); //5
        uis_.push_back(Sprite::Create(Operationui::ArrowDown, downPos, 0.0f, size)); //6
        uis_.push_back(Sprite::Create(Operationui::ArrowLeft, rightPos, 0.0f, size));//7


        // =====================
        // 3. SPACE
        // =====================
        Vector2 spaceSize = { 80.0f, 40.0f };
        float spaceY = arrowTopY + groupHeight + groupSpacing;
        // 1つ目（中央）
        Vector2 spacePos = { baseCenter.x - (spaceSize.x + keySpacing) * 0.5f, spaceY };

        // 2つ目（右側）
        Vector2 spacePos2 = { baseCenter.x + (spaceSize.x + keySpacing) * 0.5f, spaceY };

        uis_.push_back(Sprite::Create(Operationui::SPACEKey, spacePos, 0.0f, spaceSize));  //8
        uis_.push_back(Sprite::Create(Operationui::SHIFT, spacePos2, 0.0f, spaceSize)); //9
    }

    void GamePlayUI::UpdateControlUI() {
        // イベントロック中はUIの状態を更新しない
        if (isEventLocked) { return; }

        Input* input = Input::GetInstance();
        // === WASD ===
        uis_[0]->SetTexture(input->PushKey(DIK_W) ? Operationui::W_RED : Operationui::W);
        uis_[1]->SetTexture(input->PushKey(DIK_A) ? Operationui::A_RED : Operationui::A);
        uis_[2]->SetTexture(input->PushKey(DIK_S) ? Operationui::S_RED : Operationui::S);
        uis_[3]->SetTexture(input->PushKey(DIK_D) ? Operationui::D_RED : Operationui::D);

        // === Arrow ===
        uis_[4]->SetTexture(input->PushKey(DIK_UP) ? Operationui::ArrowUp_RED : Operationui::ArrowUp);
        uis_[5]->SetTexture(input->PushKey(DIK_RIGHT) ? Operationui::ArrowRight_RED : Operationui::ArrowRight);
        uis_[6]->SetTexture(input->PushKey(DIK_DOWN) ? Operationui::ArrowDown_RED : Operationui::ArrowDown);
        uis_[7]->SetTexture(input->PushKey(DIK_LEFT) ? Operationui::ArrowLeft_RED : Operationui::ArrowLeft);

        // === SPACE / SHIFT ===
        uis_[8]->SetTexture(input->PushKey(DIK_SPACE) ? Operationui::SPACEKey_RED : Operationui::SPACEKey);

        // 1. ダッシュ中
        if (player_->GetMove()->GetDashing()) {
            uis_[9]->SetTexture(Operationui::SHIFT_RED);
            // 明度100%
            uis_[9]->SetColor({ 1,1,1,1 });
        }
        // 2. クールタイム中
        else if (!player_->GetMove()->CanDash()) {
            uis_[9]->SetTexture(Operationui::SHIFT);
            // 半透明・暗く
            uis_[9]->SetColor({ 0.5f,0.5f,0.5f,0.3f });
        }
        // 3. 準備完了
        else {
            uis_[9]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
            uis_[9]->SetTexture(input->PushKey(DIK_LSHIFT) ? Operationui::SHIFT_RED : Operationui::SHIFT);
        }
    }
    
    void GamePlayUI::UpdateControlUIAnimation() {
        if (!isAnimating_) return;

        timer_ += 1.0f / 60.0f;
        float t = std::min(timer_ / duration_, 1.0f);
        float ease = EaseOutBack(t);
        if (t >= 1.0f) {
            t = 1.0f;
            isAnimating_ = false;
        }

        for (size_t i = 0; i < uis_.size(); ++i) {
            float w = Lerp(0.0f, uiOriginalSizes_[i].x, ease);
            float h = Lerp(0.0f, uiOriginalSizes_[i].y, ease);
            uis_[i]->SetSize({ w, h });
        }

        if (t >= 1.0f) isAnimating_ = false;
    }

}