#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include <Skybox.h>
#include<FadeManager.h>
#include<Titleparticle.h>
#include<Player.h>

/// <summary>
/// タイトルシーン
/// タイトル画面を管理するシーン。
/// UI 表示、タイトル演出用モデル、パーティクル、フェード制御を担当する。
/// </summary>
class TitleScene : public BaseScene {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    void Update() override;
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() override;
    /// <summary>
    /// カメラの初期化を行う
    /// </summary> 
    void InitializeCamera();
    /// <summary>
    /// テクスチャの、モデルの読み込み
    /// </summary>
    void LoadResources();
    /// <summary>
    /// UIスプライトの生成と初期化を行う
    /// </summary> 
    void InitializeUI();
    /// <summary>
    /// モデルの生成と初期化を行う
    /// </summary> 
    void InitializeModel();
    /// <summary>
    /// フェードイン開始および更新処理
    /// </summary>
    void UpdateFadeIn();
    /// <summary>
    /// 入力に応じたフェードアウト開始処理
    /// </summary>
    void UpdateFadeOut();
    /// <summary>
    /// フェード完了後のシーン遷移処理
    /// </summary>
    void UpdateSceneTransition();
    /// <summary>
    /// タイトル画面用プレイヤー演出更新
    /// ・横方向イージング移動
    /// ・上下の浮遊アニメーション
    /// </summary>
    void UpdateTitlePlayerMotion();
private: // メンバ変数
    std::unique_ptr <Sprite> ui_title_;
    std::unique_ptr <Sprite> ui_start_;
    std::vector<Sprite*> uiSprites_;

    std::unique_ptr <Player> player_;;
    // 背景オブジェクト 
    std::unique_ptr <Skybox> skybox_ = nullptr;
    // 演出用モデル
    Transform playertransform_{};
    // 移動制御
    float timer;
    float moveDuration;
    bool moveFinished;
    // 移動範囲設定 
    float titleStartX_;
    float titleEndX_;
    // フェード制御        
    float time = 0.0f;          // 経過時間 
    bool hasCheckedFade_ = false; // フェード処理済みかどうか

    std::unique_ptr <Titleparticle> particle_;


    static constexpr float kTitleMoveDuration = 240.0f;
    static constexpr float kFloatSpeed = 0.03f;
    static constexpr float kFloatAmplitude = 2.3f;

    // フェード演出用
bool isPlayerBoost_ = false;
float playerSpeedX_ = 0.0f;
float playerAccelX_ = 0.15f;   // 加速量
float playerMaxSpeedX_ = 5.0f; // 上限（任意）
};