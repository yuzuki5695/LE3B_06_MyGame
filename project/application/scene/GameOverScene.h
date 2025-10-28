#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include <Skybox.h>

struct PartInfo {
    std::unique_ptr<Object3d> obj;
    Transform transform;
    Vector3 fallSpeed;
    Vector3 rotateSpeed;
};

/// <summary>
/// ゲームオーバーシーン
/// </summary>
class GameOverScene : public BaseScene
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
    std::vector<PartInfo> partsList; // 複数のパーツ用
    int partCount_ = 7; // 生成するオブジェクト数
    std::unique_ptr <Sprite> ui1_ = nullptr;     // スプライト 
    std::unique_ptr <Skybox> Box_ = nullptr;     //　背景

};