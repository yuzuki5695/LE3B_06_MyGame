#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include <Skybox.h>

/// <summary>
/// ゲームクリアシーン
/// </summary>
class GameClearScene : public BaseScene
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
    /// 毎フレーム更新処理
    /// </summary>
    void Update() override;
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() override;
private: // メンバ変数
    std::unique_ptr <Sprite> ui1_ = nullptr;     // スプライト 
    std::unique_ptr <Object3d> clear = nullptr;  // オブジェクト
    std::unique_ptr <Skybox> Box_ = nullptr;     //　背景
    
    std::unique_ptr <Object3d> player_ = nullptr;  // オブジェクト
    Vector3 offset_;
};