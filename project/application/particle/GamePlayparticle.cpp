#include "GamePlayparticle.h"
#include <numbers>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

void GamePlayparticle::Initialize(Object3d* target) {
    // パーティクルグループ生成
    ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Resources/Particle.png", "Particle.obj", VertexType::Model);
    ParticleManager::GetInstance()->CreateParticleGroup("Particles_02", "Resources/Particle_02.png", "Particle_02.obj", VertexType::Model);
    ParticleManager::GetInstance()->CreateParticleGroup("Firework", "Resources/Particle.png", "Particle/Particle.obj", VertexType::Model);    
    ParticleManager::GetInstance()->CreateParticleGroup("Shockwave", "Resources/Shockwave.png", "Particle/Shockwave.obj", VertexType::Model);


    // パーティクルにランダム変数を組み込む[デフォルトは全て0] 
    random_
        .SetOffset({ -0.3f,0.0f,-0.1f }, { 0.3f,0.0f,0.1f })
        .SetColor(0.0f, 1.0f)
        .SetVelocity({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });

    // 発生
    particles_ = std::make_unique <FollowEmitter>(
        "Particles",                                                                           // パーティクルグループ名
        1,                                                                                     // 発生数
        Transform{ { 0.05f, 0.1f, 0.3f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },        // サイズ,回転,位置
        Vector4{ 1.0f,1.0f,1.0f,1.0f },                                                          // カラー
        0.8f,                                                                                  //  寿命
        0.0f,                                                                                  // 経過時間（基本は0から開始）
        Velocity{ {0.0f, -0.06f, -0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0} },                 // ← 風
        random_                                                                                // ランダムパラメータ（速度、回転、スケール、色などの範囲を指定）
    );

    // ターゲットを設定
    particles_->SetTarget(target);
    particles_->maxParticles_ = 400;   // 最大200個まで
    particles_->emitPerTick_ = 2;      // 1回のUpdateで2個ずつ発生
    particles_->SetOffset({ 0.0f, 0.3f, 0.0f });

    // 発生
    particles_02_ = std::make_unique <FollowEmitter>(
        "Particles_02",                                                                           // パーティクルグループ名
        1,                                                                                     // 発生数
        Transform{ { 0.05f, 0.1f, 0.3f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },        // サイズ,回転,位置
        Vector4{ 1.0f,1.0f,1.0f,1.0f },                                                          // カラー
        0.4f,                                                                                  //  寿命
        0.0f,                                                                                  // 経過時間（基本は0から開始）
        Velocity{ {0.0f, -0.06f, -0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0} },                 // ← 風
        random_
    );
    // ターゲットを設定
    particles_02_->SetTarget(target);
    particles_02_->maxParticles_ = 250;   // 最大200個まで
    particles_02_->emitPerTick_ = 1;      // 1回のUpdateで2個ずつ発生
    particles_02_->SetOffset({ 0.0f, 0.3f, 0.0f });


    explosionErandom_
        .SetOffset({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f })
        .SetScale({ 0.06f,0.06f,0.06f }, { 0.1f, 0.1f, 0.1f })
        .SetVelocity({ -0.1f, -0.1f, -0.1f }, { 0.15f,  0.2f,  0.1f });


    shockwaveErandom_
        .SetRotate({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,360.0f }) // Z軸にランダム回転
        .SetScale({ 0.02f,0.02f,0.02f }, { 0.1f,0.1f,0.1f }) // 初期サイズにランダム差 
        .SetLifetime(0.3f, 0.5f)          // 短い寿命でフェードアウト
        .SetVelocity(
            Velocity{
                {0.0f,0.0f,0.0f},  // 位置の移動はなし
                {0.0f,0.0f,0.0f},  // 回転なし
                {0.006f,0.006f,0.006f} // scale が増える速度
            },
            Velocity{
                {0.0f,0.0f,0.0f},
                {0.0f,0.0f,0.0f},
                {0.06f,0.06f,0.06f} // 最大速度
            }
        );
}

void GamePlayparticle::Update() {
    particles_->Update();
    particles_02_->Update();



    // ヒット座標キュー処理
    for (const Vector3& pos : hitPositions_) {
        // 爆発エミッタ生成（敵がやられた瞬間だけ）
        auto explosion = std::make_unique<DefaultParticleEmitter>(
            "Firework", 5,
            Transform{ {0.0f,0.0f,0.0f},{0,0,0}, pos },
            Vector4{ 1,0.6f,0.2f,1 }, 0.0f, 0.0f,
            Velocity{ {0,0,0},{0,0,0},{0,0,0} }, explosionErandom_
        );
        explosion->SetLifeTime(1.0f);
        explosion->SetEmitCount(30);
        explosionEmitters_.push_back(std::move(explosion));

        auto shockwave = std::make_unique<DefaultParticleEmitter>(
            "Shockwave", 1,
            Transform{ {0,0,0},{0,0,0}, pos },
            Vector4{ 1,1,1,1 }, 0.0f, 0.0f,
            Velocity{ {0,0,0},{0,0,0},{0,0,0} }, shockwaveErandom_
        );
        shockwave->SetLifeTime(0.7f);
        shockwave->SetEmitCount(5);
        shockwaveEmitters_.push_back(std::move(shockwave));
    }
    hitPositions_.clear();

    // すべての爆発パーティクルを更新
    explosionEmitters_.erase(
        std::remove_if(explosionEmitters_.begin(), explosionEmitters_.end(),
            [](auto& e) {
                e->Update();
                return e->IsDead();
            }),
        explosionEmitters_.end()
    );

    shockwaveEmitters_.erase(
        std::remove_if(shockwaveEmitters_.begin(), shockwaveEmitters_.end(),
            [](auto& e) {
                e->Update();
                return e->IsDead();
            }),
        shockwaveEmitters_.end()
    );







#ifdef USE_IMGUI
    particles_->DrawImGuiUI();
#endif // USE_IMGUI

}