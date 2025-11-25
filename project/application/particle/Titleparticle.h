#pragma once
#include <ParticleManager.h>
#include <ParticleEmitter.h>

class Titleparticle {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    void Update();

private: // メンバ変数
    // エミッター生成
    std::unique_ptr <ParticleEmitter> circle_;
    // ランダムパラメータ
    RandomParameter random_;

};