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
        // UIテクスチャ一覧
        const std::array<const char*, 29> operationTextures = {
            Operationui::W,Operationui::A,Operationui::S,Operationui::D,
            Operationui::ArrowUp,Operationui::ArrowLeft,Operationui::ArrowDown,
            Operationui::ArrowRight,Operationui::SPACEKey,Operationui::SHIFT,Operationui::W_RED,
            Operationui::A_RED,Operationui::S_RED,Operationui::D_RED,Operationui::ArrowUp_RED,
            Operationui::ArrowLeft_RED,Operationui::ArrowDown_RED,Operationui::ArrowRight_RED,Operationui::SPACEKey_RED,
            Operationui::SHIFT_RED,Ui::Gage,Ui::Player_ui,Operationui::LevelGage_Frame,Operationui::LevelGage_Green,
            Operationui::LevelGage_Yellow,Event::start,Operationui::Levelup,Operationui::Levelmax,Ui::Player_Hp
        };


        // UIをまとめて読み込み
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
        // 背景
        expBarBack_ = Sprite::Create(Operationui::LevelGage_Frame, { 0.0f, 0.0f }, 0.0f, { 80.0f, 20.0f });
        expBarBack_->SetColor({ 1,1,1,0 });
        // ゲージ部分
        expBarFill_ = Sprite::Create(Operationui::LevelGage_Green, { 0.0f, 0.0f }, 0.0f, { 80.0f, 20.0f });
        expBarFill_->SetColor({ 1,1,1,0 });
        isExpBarVisible_ = false;
        expBarAlpha_ = 1.0f;         // 最大明度
        expBarDuration_ = 2.0f;     // 表示時間
        expBarTimer_ = expBarDuration_; // タイマー(初回は最大にしておく)
        isEventLocked = true;

        levelup_ = Sprite::Create(Operationui::Levelup, { 0.0f, 0.0f }, 0.0f, { 150.0f, 100.0f });
        levelup_->SetAnchorPoint({ 0.5f, 0.5f });
        isLevelUpVisible_ = false;
        levelUpAlpha_ = 0.0f;
        levelUpTimer_ = 0.0f;
    }
    
    void GamePlayUI::SetPlayerHp() {
        // プレイヤーの参照があることを確認してHPアイコンを生成       
        uint32_t maxHp = player_->GetMaxHP(); // プレイヤーから最大HPを取得
        hpHearts_.clear();
        for (uint32_t i = 0; i < maxHp; ++i) {
            // 100, 15 の位置から 100 ずつ X 軸をずらす
            float posX = kHeartStartX + (i * kHeartOffsetX);
            Vector2 position = { posX, kHeartY };
            // スプライト生成
            auto heart = Sprite::Create(Ui::Player_Hp, position, 0.0f, kHeartSize);
            hpHearts_.push_back(std::move(heart));
        }
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
        // EXPバーのフェード更新
        UpdateExpBarFade();
        // レベルアップUIの更新
        UpdateLevelUpFade();
        gage_->Update();
        player_ui_->Update();
        expBarBack_->Update();
        expBarFill_->Update();
        levelup_->Update();
        for (std::unique_ptr<Sprite>& ui : uis_) {
            ui->Update();
        }

        // HP表示の更新処理
        uint32_t currentHp = player_->GetHP();
        for (size_t i = 0; i < hpHearts_.size(); ++i) {
            if (i < currentHp) {
                // 現在のHP以下なら通常表示
                hpHearts_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            } else {
                // 現在のHPを超えている（ダメージを受けた）分は非表示（透明）にする
                hpHearts_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
            }
            // スプライト自体のマトリクス更新
            hpHearts_[i]->Update();
        }
        // 既存のuis_などのUpdate処理...
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
        if (isExpBarVisible_) {
            // プレイヤーに合わせて表示するUIの描画
            expBarBack_->Draw();
            expBarFill_->Draw();
        }
        if (isLevelUpVisible_) {
            levelup_->Draw();
        }
        // 操作UIの描画
        for (std::unique_ptr<Sprite>& ui : uis_) {
            ui->Draw();
        }
        // イベントロック中などに非表示にしたい場合は条件を追加してください
        for (const auto& heart : hpHearts_) {
            heart->Draw();
        }
    }

    void GamePlayUI::UpdateLevelUpFade() {
        if (!isLevelUpVisible_) {
            return;
        }
        constexpr float fadeDuration = 2.0f;
        levelUpTimer_ += 1.0f / 60.0f;
        float t = levelUpTimer_ / fadeDuration;
        t = std::clamp(t, 0.0f, 1.0f);
        levelUpAlpha_ = 1.0f - t;
        levelup_->SetColor({ 1.0f, 1.0f, 1.0f, levelUpAlpha_ });
        float scale = Lerp(1.3f, 1.0f, t);
        if (t >= 1.0f) {
            isLevelUpVisible_ = false;
            levelup_->SetColor({ 1,1,1,0 });
        }
    }

    void GamePlayUI::UpdatePlayerFollowUI() {
        if (!player_) { return; }
        // カメラの取得
        Camera* camera = CameraManager::GetInstance()->GetActiveCamera();

        if (!camera) { return; }
        // プレイヤー頭上
        Vector3 worldPos = player_->GetTranslate();
        worldPos.y += 2.0f;
        // ワールド座標をNDC座標に変換
        Matrix4x4 view = camera->GetViewMatrix();
        // プロジェクション行列の取得
        Matrix4x4 projection = camera->GetProjectionMatrix();
        // ビュー行列とプロジェクション行列を掛け合わせて、ワールド座標をNDC座標に変換するための行列を作成
        Matrix4x4 vp = Multiply(view, projection);
        // ワールド座標をNDC座標に変換する関数を呼び出す
        Vector3 ndcPos = TransformPoint(worldPos, vp);
        // NDC座標をスクリーン座標に変換
        constexpr float screenWidth = 1280.0f;
        constexpr float screenHeight = 720.0f;
        // NDC座標は[-1, 1]の範囲なので、スクリーン座標に変換する
        Vector2 screenPos;
        screenPos.x = (ndcPos.x + 1.0f) * 0.5f * screenWidth;
        screenPos.y = (1.0f - ndcPos.y) * 0.5f * screenHeight;
        // UI表示位置補正
        Vector2 offset = { -40.0f,15.0f };
        Vector2 finalPos = screenPos + offset;
        // 位置更新
        expBarBack_->SetPosition(finalPos);               
        expBarFill_->SetPosition({ finalPos.x,finalPos.y });
        // レベルアップUIの座標更新
        levelup_->SetPosition({ finalPos.x + 40.0f, finalPos.y - 40.0f });
        // 経験値割合
        constexpr float kMaxBarWidth = 80.0f;
        constexpr float kBarHeight = 20.0f;
        float expRatio = static_cast<float>(player_->GetExp()) / static_cast<float>(player_->GetNextLevelExp());
        expRatio = std::clamp(expRatio, 0.0f, 1.0f);
        // サイズ変更
        float currentWidth = kMaxBarWidth * expRatio;
        expBarFill_->SetSize({ currentWidth,kBarHeight });
        // 最大時黄色化
        if (expRatio >= 1.0f) {
            expBarFill_->SetTexture(Operationui::LevelGage_Yellow);
        } else {
            expBarFill_->SetTexture(Operationui::LevelGage_Green);
        }
    } 

    void GamePlayUI::ShowExpBar() {
        isExpBarVisible_ = true;
        // 明度最大
        expBarAlpha_ = 1.0f;
        // タイマー初期化
        expBarTimer_ = 0.0f;
        // 即反映
        expBarBack_->SetColor({ 1.0f,1.0f,1.0f,expBarAlpha_ });
        expBarFill_->SetColor({ 1.0f,1.0f,1.0f,expBarAlpha_ });
    }

    void GamePlayUI::ShowLevelUp(bool isMaxLevel) {
        isLevelUpVisible_ = true;
        levelUpAlpha_ = 1.0f;
        levelUpTimer_ = 0.0f;
        // 初期サイズは1.3倍
        levelup_->SetSize({ 150.0f,100.0f });
        levelup_->SetColor({ 1,1,1,1 });
        // 表示位置記録
        levelUpBasePos_ = levelup_->GetPosition();
        if (isMaxLevel) {
            levelup_->SetTexture(Operationui::Levelmax);
        } else {
            levelup_->SetTexture(Operationui::Levelup);
        }
    }

    void GamePlayUI::UpdateExpBarFade() {
        if (!isExpBarVisible_) { return; }
        constexpr float fadeDuration = 2.0f;
        expBarTimer_ += 1.0f / 60.0f;
        float t = expBarTimer_ / fadeDuration;
        t = std::clamp(t, 0.0f, 1.0f);

        // 1 → 0
        expBarAlpha_ = 1.0f - t;
        expBarBack_->SetColor({ 1,1,1,expBarAlpha_ });
        expBarFill_->SetColor({ 1,1,1,expBarAlpha_ });

        // 完全終了
        if (t >= 1.0f) {
            isExpBarVisible_ = false;
            expBarAlpha_ = 0.0f;
            expBarBack_->SetColor({ 1,1,1,0 });
            expBarFill_->SetColor({ 1,1,1,0 });
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