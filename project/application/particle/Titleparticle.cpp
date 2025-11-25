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
        //    .SetRotateZ(-std::numbers::pi_v<float>, std::numbers::pi_v<float>)
        .SetColor(0.0f, 1.0f)
        .SetOffset({ 0.0f,0.0f,0.0f }, { 1.0f, 1.0f, 1.0f });

    // 発生
    particles_ = std::make_unique <FollowEmitter>(
        "Particles",                                                                           // パーティクルグループ名
        2,                                                                                     // 発生数
        Transform{ { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 30.0f } },        // サイズ,回転,位置
        Vector4{ 1.0f,1.0f,1.0f,1.0f },                                                          // カラー
        1.5f,                                                                                  // 発生周期 or 寿命（自由に定義可能）
        0.0f,                                                                                  // 経過時間（基本は0から開始）
        Velocity{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0} },                 // ← 風
        random_                                                                                // ランダムパラメータ（速度、回転、スケール、色などの範囲を指定）
    );

    // ターゲットを設定
    particles_->SetTarget(target);
}

void Titleparticle::Update() {
    particles_->Update();

    particles_->DrawImGuiUI();
}