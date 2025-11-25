#pragma once
#include <ParticleManager.h>
#include <ParticleEmitter.h>
#include<FollowEmitter.h>

// タイトル専用パーティクル
class GameClearparticle {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(Object3d* target);
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    void Update();
        
    void SetVelocity(const Velocity& v) { if (particles_) particles_->SetVelocity(v); }

private: // メンバ変数
    // エミッター生成
    std::unique_ptr <FollowEmitter> particles_;
    // ランダムパラメータ
    RandomParameter random_;

};