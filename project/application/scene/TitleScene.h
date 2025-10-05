#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include <Skybox.h>

// ゲームプレイシーン
class TitleScene : public BaseScene
{
public: // メンバ関数
    // 初期化
    void Initialize() override;
    // 終了
    void Finalize() override;
    // 毎フレーム更新
    void Update() override;
    // 描画
    void Draw() override;
private: // メンバ変数
    std::unique_ptr <Sprite> ui1_;
    std::unique_ptr <Sprite> ui2_;

    std::unique_ptr <Object3d> title_; 
    std::unique_ptr <Skybox> Box_ = nullptr;

    std::unique_ptr <Object3d> tile_; 
     
    std::unique_ptr <Object3d> player_; 
    Transform playertransform_{};
    
    float timer;
    float moveDuration;   
    bool moveFinished;


    float startX;
    float endX;

};