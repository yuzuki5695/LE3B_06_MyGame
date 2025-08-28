#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include <Skybox.h>

// ゲームクリアシーン
class GameOverScene : public BaseScene
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
            
            
    std::unique_ptr <Object3d> clear = nullptr;
           
    std::unique_ptr <Skybox> Box_ = nullptr;
};