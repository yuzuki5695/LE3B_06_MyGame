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

        gage_ = Sprite::Create(Ui::Gage, Vector2{ 380.0f, 10.0f }, 0.0f, Vector2{ 500.0f,30.0f });
        player_ui_ = Sprite::Create(Ui::Player_ui, Vector2{ 380.0f, 12.3f }, 0.0f, Vector2{ 25.0f,25.0f });

        // 2. UI生成
        Vector2 size = { 40.0f, 40.0f };
        Vector2 center = { 85.0f, 470.0f };
        CreateWASDUI(center, size, 4.0f, 25.0f);

        // 3. 初期状態設定（サイズ0）
        for (auto& ui : uis_) {
            uiOriginalSizes_.push_back(ui->GetSize());
            ui->SetAnchorPoint({ 0.5f, 0.5f });
            ui->SetSize({ 0.0f, 0.0f });
        }

        isAnimating_ = true;
        timer_ = 0.0f;
        duration_ = 0.5f;

        pausemenu_ = std::make_unique<Pausemenu>();
        pausemenu_->Initialize();
    }

    void GamePlayUI::Update() {
        UpdateStageProgressUI();

        gage_->Update();
        player_ui_->Update();

        UpdateControlUIAnimation();
        UpdateControlUI();
        for (std::unique_ptr<Sprite>& ui : uis_) {
            ui->Update();
        }
    }

    void GamePlayUI::Draw() {


        gage_->Draw();
        player_ui_->Draw();

        for (std::unique_ptr<Sprite>& ui : uis_) {
            ui->Draw();
        }
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

        if (!player_) return;
        bool canDash = player_->GetMove()->GetDashing();

        if (!canDash) {
            // 【クールタイム中 または ダッシュ演出中】
            // キー入力に関係なく、強制的に「赤（使用不可/リキャスト中）」を表示
            uis_[9]->SetTexture(Operationui::SHIFT_RED);
            uis_[9]->SetColor({ 0.3f, 0.3f, 0.3f, 1.0f });
        } else {
            // 【ダッシュ準備完了】
            uis_[9]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            // 通常時：SHIFT（白など）、押している間：SHIFT_RED（赤）
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