#include "Titleparticle.h"
#include <numbers>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

void Titleparticle::Initialize(Object3d* target) {
    // パーティクルグループ生成
    ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Resources/uvChecker.png", "Particle.obj", VertexType::Model);

    // パーティクルにランダム変数を組み込む[デフォルトは全て0] 
    random_
        // .SetRotateZ(-std::numbers::pi_v<float>, std::numbers::pi_v<float>)
        .SetOffset({ 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.1f })
        .SetColor(0.0f, 1.0f)
        .SetVelocity({ -0.13f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });

    // 発生
    particles_ = std::make_unique <FollowEmitter>(
        "Particles",                                                                           // パーティクルグループ名
        1,                                                                                     // 発生数
        Transform{ { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 30.0f } },        // サイズ,回転,位置
        Vector4{ 1.0f,1.0f,1.0f,1.0f },                                                          // カラー
        1.5f,                                                                                  //  寿命
        0.0f,                                                                                  // 経過時間（基本は0から開始）
        Velocity{ {-0.06f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0} },                 // ← 風
        random_                                                                                // ランダムパラメータ（速度、回転、スケール、色などの範囲を指定）
    );

    // ターゲットを設定
    particles_->SetTarget(target);
    particles_->maxParticles_ = 200;   // 最大200個まで
    particles_->emitPerTick_ = 1;      // 1回のUpdateで2個ずつ発生
}

void Titleparticle::Update() {
    particles_->Update();

#ifdef USE_IMGUI
    particles_->DrawImGuiUI();
#endif // USE_IMGUI

}