#include "StartEvent.h"
#include <MatrixVector.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <CameraManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID;
using namespace MyGame::Event;

namespace MyGame {

    void StartEvent::Initialize() {
        // テクスチャのロード
        TextureManager::GetInstance()->LoadTexture(Textures::Event::Black);
        TextureManager::GetInstance()->LoadTexture(Textures::Event::Startevent_01);
        TextureManager::GetInstance()->LoadTexture(Textures::Event::Startevent_02);
        TextureManager::GetInstance()->LoadTexture(Textures::Event::start_mission);
        TextureManager::GetInstance()->LoadTexture(Textures::Event::start);

        sprites_.clear();
        // データ登録（ここ重要）
        sprites_.push_back({ nullptr, {}, 0.0f, SpriteID::Background });
        sprites_.push_back({ nullptr, {}, 0.0f, SpriteID::TopBar });
        sprites_.push_back({ nullptr, {}, 0.0f, SpriteID::BottomBar });
        sprites_.push_back({ nullptr, {}, 0.0f, SpriteID::Mission });

        for (SpriteUnit& mission_ : sprites_) {

            switch (mission_.id) {
            case SpriteID::Background:
                mission_.mission_ = Sprite::Create(Textures::Event::Black, Vector2{ 0.0f, 300.0f }, 0.0f, Vector2{ 1280.0f,150.0f });
                mission_.alpha = 0.0f;
                mission_.mission_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, mission_.alpha });
                break;
            case SpriteID::TopBar:
                mission_.mission_ = Sprite::Create(Textures::Event::Startevent_02, Vector2{ -1280.0f, 300.0f }, 0.0f, Vector2{ 1280.0f, 75.0f });
                mission_.alpha = 0.0f;
                break;
            case SpriteID::BottomBar:
                mission_.mission_ = Sprite::Create(Textures::Event::Startevent_01, Vector2{ 1280.0f, 375.0f }, 0.0f, Vector2{ 1280.0f, 75.0f });
                mission_.alpha = 0.0f;
                break;

            case SpriteID::Mission:
                mission_.mission_ = Sprite::Create(Textures::Event::start_mission, Vector2{ 500.0f, 265.0f }, 0.0f, Vector2{ 300.0f,200.0f });
                mission_.alpha = 0.0f;
                mission_.mission_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, mission_.alpha });
                break;
            }
        }

        // ゲーム開始イベントの初期化
        phase_ = EventPhase::Phase1;

        closeSpeed_ = 17.0f;

        timefige_ = false;
        isFinished_ = false;
    }

    void StartEvent::Update() {
        SpriteUnit* back = FindSprite(SpriteID::Background);
        SpriteUnit* mission = FindSprite(SpriteID::Mission);
        SpriteUnit* top = FindSprite(SpriteID::TopBar);
        SpriteUnit* bottom = FindSprite(SpriteID::BottomBar);

        // 演出フェーズごとに処理を分岐
        switch (phase_) {
        case EventPhase::Phase1: // フェーズ1：黒背景のフェードイン
            phaseDuration_ = 1.0f; // 1秒かける
            back->alpha += 0.01f;  // 少しずつ明るくする
            back->mission_->SetColor({ 1.0f, 1.0f, 1.0f,  back->alpha }); // 色（RGBA）を更新

            if (back->alpha >= 0.7f) {
                back->alpha = 0.7f;
                phase_ = EventPhase::Phase2;
                progress_ += phaseDuration_;
            }
            break;
        case EventPhase::Phase2: // フェーズ3：MISSION文字のフェードイン          
            phaseDuration_ = 0.5f; // 0.5秒で閉じる

            // 上側の移動
            top->position = top->mission_->GetPosition();
            if (top->position.x < 200.0f) { // 左上座標を軸として目的の値の達するまで右へ移動
                top->position.x += closeSpeed_;
                if (top->position.x > 0.0f)   top->position.x = 0.0f; // はみ出し防止
            }

            // 下側の移動
            bottom->position = bottom->mission_->GetPosition();
            if (bottom->position.x > 0.0f) { // 左上座標を軸として目的の値の達するまで左へ移動
                bottom->position.x -= closeSpeed_;
                if (bottom->position.x < 0.0f) bottom->position.x = 0.0f; // はみ出し防止
            }

            // 反映
            top->mission_->SetPosition(top->position);
            bottom->mission_->SetPosition(bottom->position);

            // 両方とも中央に到達したら次のフェーズへ
            if (top->position.x == 0.0f && bottom->position.x == 0.0f) {
                phase_ = EventPhase::Phase3;
                progress_ += phaseDuration_;
            }
            break;
        case EventPhase::Phase3: // フェーズ3：MISSION文字のフェードイン
            phaseDuration_ = 0.3f;
            mission->alpha += 0.05f;  // 少しずつ明るくする

            if (mission->alpha >= 1.0f) {
                mission->alpha = 1.0f;
                timefige_ = true;
                phase_ = EventPhase::Phase4;
                progress_ += phaseDuration_;
            }

            mission->mission_->SetColor({ 1.0f, 1.0f, 1.0f, mission->alpha }); // 色を更新
            break;
        case EventPhase::Phase4: // フェーズ4：MISSION表示維持 → フェードアウト → “START”表示
            phaseDuration_ = 2.0f; // 2秒間維持
            // 明るくなってから時間をカウント
            missionTimer_ += 1.0f / 60.0f; // 60fps換算（毎フレーム約0.016秒）
            if (missionTimer_ >= 1.0f && timefige_) {
                // 5秒経過したらフェードアウト開始
                mission->alpha -= 0.05f;
            }

            // 消え切ったら次のフェーズへ
            if (mission->alpha <= 0.0f) {
                mission->alpha = 1.0f;
                timefige_ = false;
                mission->mission_->SetTexture(Textures::Event::start);
                phase_ = EventPhase::End;
                progress_ += phaseDuration_;
            }
            mission->mission_->SetColor({ 1.0f, 1.0f, 1.0f,  mission->alpha }); // 色（RGBA）を更新
            break;
        case EventPhase::End: // フェーズEnd：バーが外へ開いて終了       
            phaseDuration_ = 0.5f;
            // 上側の移動
            if (top->position.x < 1280.0f) { // さらにspriteの大きさ分移動
                top->position.x += closeSpeed_;
                if (top->position.x > 1280.0f)  top->position.x = 1280.0f;
            }
            // 下側の移動
            if (bottom->position.x > -1280.0f) { // さらにspriteの大きさ分移動
                bottom->position.x -= closeSpeed_;
                if (bottom->position.x < -1280.0f) bottom->position.x = -1280.0f;
            }
            // 両方が完全に外に出たらフェードアウト完了
            if (top->position.x >= 1280.0f && bottom->position.x <= -1280.0f) {
                mission->alpha -= 0.05f;
                if (mission->alpha <= 0.7f)
                    back->alpha -= 0.05f;
                if (back->alpha <= 0.0f && mission->alpha <= 0.0f) {
                    isFinished_ = true;
                }
            }

            // 反映
            top->mission_->SetPosition(top->position);
            bottom->mission_->SetPosition(bottom->position);
            back->mission_->SetColor({ 1.0f, 1.0f, 1.0f, back->alpha });
            mission->mission_->SetColor({ 1.0f, 1.0f, 1.0f, mission->alpha });
            break;
        }

        // 各スプライトの更新
        for (SpriteUnit& mission_ : sprites_) {
            mission_.mission_->Update();
        }
    }
    
    void StartEvent::Draw2D() {
        for (SpriteUnit& mission_ : sprites_) {
            mission_.mission_->Draw();
        }
    }

    SpriteUnit* StartEvent::FindSprite(SpriteID id) {
        // 指定されたIDを持つスプライトを検索して返す
        for (SpriteUnit& mission_ : sprites_) {
            if (mission_.id == id) return &mission_;
        }
        return nullptr;
    }
}