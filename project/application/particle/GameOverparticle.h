#pragma once
#include <ParticleManager.h>
#include <ParticleEmitter.h>
#include<FollowEmitter.h>

// タイトル専用パーティクル
class GameOverparticle {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(const std::vector<Object3d*>& targets);
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    void Update();

    void Finalize();

private: // メンバ変数
    // エミッター生成
    std::vector<std::unique_ptr<FollowEmitter>> emitters_;
    // ランダムパラメータ
    RandomParameter random_;
    std::vector<std::unique_ptr<FollowEmitter>> emitters2_;
};