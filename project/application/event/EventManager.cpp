#include "EventManager.h"
#include <MatrixVector.h>
#include<TextureManager.h>
#include <ModelManager.h>
#include <CameraManager.h>
#include<Input.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

using namespace MatrixVector;

// シングルトン用インスタンス
EventManager* EventManager::instance = nullptr;

///====================================================
/// シングルトンインスタンスの取得
///====================================================
EventManager* EventManager::GetInstance() {
    // まだインスタンスが生成されていなければ作成
    if (instance == nullptr) {
        instance = new EventManager;
    }
    return instance;
}

///====================================================
/// 終了処理
///====================================================
void EventManager::Finalize() {
    // インスタンスを削除してnullptrに戻す
    delete instance;
	instance = nullptr;   
}

///====================================================
/// 初期化処理
///====================================================
void EventManager::Initialize(const std::string& stateName) {
    // テクスチャのロード
    TextureManager::GetInstance()->LoadTexture("Event/Black.png");
    TextureManager::GetInstance()->LoadTexture("Event/Startevent_01.png");
    TextureManager::GetInstance()->LoadTexture("Event/Startevent_02.png");
    TextureManager::GetInstance()->LoadTexture("Event/mission.png");
    TextureManager::GetInstance()->LoadTexture("Event/start.png");
    // 小文字化（大文字小文字を区別しないため）
    std::string lower = stateName;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    // 文字列によってstateを設定
    if (lower == "none") {
        state_ = EventState::None;
        // 初期化
        sprite_ = Sprite::Create("Event/Black.png", Vector2{ 0.0f, 300.0f }, 0.0f, Vector2{ 1280.0f,150.0f });
        sprite_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 0.0f });
        alpha_ = 0.0f;
        // スプライトサイズ
        size_ = { 1280.0f, 75.0f }; // 画面幅1280、高さ半分ずつくらいを想定
        // 左から右に動くスプライト（上側）
        topSprite_ = Sprite::Create("Event/Startevent_02.png", Vector2{ -1280.0f, 300.0f }, 0.0f, size_);  
        topSprite_->SetTextureSize(Vector2{ 1280.0f,75.0f }); 
        // 右から左に閉じるスプライト（下側）
        bottomSprite_ = Sprite::Create("Event/Startevent_01.png", Vector2{ 1280.0f, 375.0f }, 0.0f, size_);
        bottomSprite_->SetTextureSize(Vector2{ 1280.0f,75.0f });
        closeSpeed_ = 17.0f;

        missionsize_ = { 300.0f,200.0f };
        mission_ = Sprite::Create("Event/mission.png", Vector2{ 500.0f, 265.0f }, 0.0f, missionsize_);
        missionalpha_ = 0.0f;
        mission_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, missionalpha_ });
        mission_->SetTextureSize(Vector2{ 300.0f,200.0f });

        timefige_ = false;
        topPos_ = {0.0f,0.0f};
        bottomPos_ = {0.0f,0.0f};

    } else if (lower == "gamestart") {
        state_ = EventState::GameStart;
    }
}

///====================================================
/// 更新処理
///====================================================
void EventManager::Update() {
    if (state_ == EventState::None) {
        sprite_->Update();
        topSprite_->Update();
        bottomSprite_->Update();
        mission_->Update();

        switch (phase_) {
        case EventPhase::Phase1:
            alpha_ += 0.01f;  // 少しずつ明るくする
            sprite_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ }); // 色（RGBA）を更新

            if (alpha_ >= 0.7f) {
                alpha_ = 0.7f;
                phase_ = EventPhase::Phase2;
            }
            break;
        case EventPhase::Phase2:
            // 上側の移動
            topPos_ = topSprite_->GetPosition();
            if (topPos_.x < 200.0f) { // 左上座標を軸として目的の値の達するまで右へ移動
                topPos_.x += closeSpeed_;
                if (topPos_.x > 0.0f) topPos_.x = 0.0f; // はみ出し防止
            }

            // 下側の移動
            bottomPos_ = bottomSprite_->GetPosition();
            if (bottomPos_.x > 0.0f) { // 左上座標を軸として目的の値の達するまで左へ移動
                bottomPos_.x -= closeSpeed_;
                if (bottomPos_.x < 0.0f) bottomPos_.x = 0.0f; // はみ出し防止
            }

            // 反映
            topSprite_->SetPosition(topPos_);
            bottomSprite_->SetPosition(bottomPos_);

            // 両方とも中央に到達したら次のフェーズへ
            if (topPos_.x == 0.0f && bottomPos_.x == 0.0f) {
                phase_ = EventPhase::Phase3;
            }

            break;
        case EventPhase::Phase3:
            missionalpha_ += 0.05f;  // 少しずつ明るくする

            if (missionalpha_ >= 1.0f) {
                missionalpha_ = 1.0f;
                timefige_ = true;
                phase_ = EventPhase::Phase4;
            }

            mission_->SetColor({ 1.0f, 1.0f, 1.0f, missionalpha_ }); // 色（RGBA）を更新
            break;
        case EventPhase::Phase4:
            // 明るくなってから時間をカウント
            missionTimer_ += 1.0f / 60.0f; // 60fps換算（毎フレーム約0.016秒）
            if (missionTimer_ >= 1.0f && timefige_) {
                // 5秒経過したらフェードアウト開始
                missionalpha_ -= 0.05f;
            }

            // 消え切ったら次のフェーズへ
            if (missionalpha_ <= 0.0f) {
                missionalpha_ = 1.0f;
                timefige_ = false;
                mission_->SetTexture("Event/start.png");
                phase_ = EventPhase::End;
            }
            mission_->SetColor({ 1.0f, 1.0f, 1.0f, missionalpha_ }); // 色（RGBA）を更新
            break;
        case EventPhase::End:
            // 上側の移動
            if (topPos_.x < 1280.0f) { // さらにspriteの大きさ分移動
                topPos_.x += closeSpeed_;
                if (topPos_.x > 1280.0f) topPos_.x = 1280.0f;
            }
            // 下側の移動
            if (bottomPos_.x > -1280.0f) { // さらにspriteの大きさ分移動
                bottomPos_.x -= closeSpeed_;
                if (bottomPos_.x < -1280.0f) bottomPos_.x = -1280.0f;
            }

            if (topPos_.x >= 1280.0f && bottomPos_.x <= -1280.0f) {
                missionalpha_ -= 0.05f;
                if(missionalpha_ <=0.7f)
                alpha_ -= 0.05f;
                if (alpha_ <= 0.0f && missionalpha_ <= 0.0f) {
                    state_ = EventState::GameStart;
                }
            }

            // 反映
            topSprite_->SetPosition(topPos_);
            bottomSprite_->SetPosition(bottomPos_);
            sprite_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });
            mission_->SetColor({ 1.0f, 1.0f, 1.0f, missionalpha_ });
            break;
        }

    } else if (state_ == EventState::GameStart) {

    }
}
	
///====================================================
/// 描画処理(3D)
///====================================================
void  EventManager::Drawo3Dbject(){
    if (state_ == EventState::None) {




    } else if (state_ == EventState::GameStart) {

    }
}

///====================================================
/// 描画処理(2D)
///====================================================
void  EventManager::Draw2DSprite(){
    if (state_ == EventState::None) {
        sprite_->Draw();
        topSprite_->Draw();
        bottomSprite_->Draw();
        mission_->Draw();


    } else if (state_ == EventState::GameStart) {

    }
}

void EventManager::DrawImGui() {
#ifdef USE_IMGUI
    ImGui::Begin("EventManager Debug");

    // 現在のイベント状態を表示
    const char* stateNames[] = { "None", "GameStart" };
    ImGui::Text("Current State: %s", stateNames[(int)state_]);

    // 現在のフェーズを表示
    const char* phaseNames[] = {
        "Phase1 (Start)",
        "Phase2",
        "Phase3",
        "Phase4",
        "End"
    };
    ImGui::Text("Current Phase: %s", phaseNames[(int)phase_]);

    // 任意で各種変数の値も確認できる
    ImGui::Separator();
    ImGui::Text("Alpha: %.2f", alpha_);
    ImGui::Text("Mission Alpha: %.2f", missionalpha_);
    ImGui::Text("Mission Timer: %.2f", missionTimer_);
    ImGui::Text("Close Speed: %.2f", closeSpeed_);
//    ImGui::Checkbox("Freeze Time", &timefige_); // デバッグ用トグル

    ImGui::End();
#endif // USE_IMGUI
}