#include "particle.h"
#include <numbers>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

void particle::Initialize() {
    // パーティクルグループ生成
    ParticleManager::GetInstance()->CreateParticleGroup("Particles", "Resources/uvChecker.png", "Particle.obj", VertexType::Model);  
    ParticleManager::GetInstance()->CreateParticleGroup("Circle", "Resources/circle2.png", "plane.obj", VertexType::Model);                 // モデルで生成

    random_ = { 
        //座標
        {0.0f,0.0f,0.0f},  // 最小
		{0.0f,0.0f,0.0f},  // 最大
        // 回転
        {0.0f,0.0f,-std::numbers::pi_v<float>},  // 最小
        {0.0f,0.0f, std::numbers::pi_v<float>},  // 最大
        // サイズ
        {1.0f,1.0f,1.0f}, // 最小
        {1.0f,1.0f,1.0f}, // 最大
        // カラー
        0.0f,  // 最小
        1.0f,  // 最大
        // 寿命
        0.0f, // 最小
        0.0f, // 最大
        // 速度
        {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},// 最小
        {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}} // 最大
    };

    // 発生
    circle_ = std::make_unique <ParticleEmitter>(
        "Circle",                                                                              // パーティクルグループ名
        8,                                                                                     // 発生数
		Transform{ { 0.05f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 2.0f, 0.0f } },        // サイズ,回転,位置
        Vector4{1.0f,1.0f,1.0f,1.0f},                                                          // カラー
        1.5f,                                                                                  // 発生周期 or 寿命（自由に定義可能）
        0.0f,                                                                                  // 経過時間（基本は0から開始）
        Velocity{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0} },                 // ← 風
		random_                                                                                // ランダムパラメータ（速度、回転、スケール、色などの範囲を指定）
    );
    circle_->Emit();
}

void particle::Update() {

//    circle_->Update();


#ifdef USE_IMGUI
    //    ParticleManager::GetInstance()->DebugUpdata();

    if (circle_) {
        circle_->DrawImGuiUI();
    }

#endif // USE_IMGUI
}

void particle::Draw() {


}