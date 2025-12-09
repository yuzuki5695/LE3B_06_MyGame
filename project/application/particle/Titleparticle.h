#pragma once
#include <ParticleManager.h>
#include <ParticleEmitter.h>
#include<FollowEmitter.h>

// タイトル専用パーティクル
class Titleparticle {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(Object3d* target);
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    void Update();

private: // メンバ変数
    // エミッター生成
    std::unique_ptr <FollowEmitter> particles_;
    // ランダムパラメータ
    RandomParameter random_;
    // エミッター生成
    std::unique_ptr <FollowEmitter> particles_02_;

};