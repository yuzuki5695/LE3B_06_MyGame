#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include <Skybox.h>
#include<FadeManager.h>

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public BaseScene
{
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
private: // メンバ変数
    std::unique_ptr <Sprite> ui1_;
    std::unique_ptr <Sprite> ui2_;
    std::unique_ptr <Sprite> ui3_;
     // 背景オブジェクト 
    std::unique_ptr <Skybox> Box_ = nullptr;
    // 演出用モデル
    std::unique_ptr <Object3d> player_; 
    Transform playertransform_{};
	// 移動制御
    float timer;
    float moveDuration;   
    bool moveFinished;
    // 移動範囲設定 
    float startX;
    float endX;  
    // フェード制御        
    float time = 0.0f;          // 経過時間 
    bool hasCheckedFade_ = false; // フェード処理済みかどうか
};