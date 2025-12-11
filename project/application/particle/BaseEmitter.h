#pragma once
#include<Vector3.h>
#include <string>
#include <vector>
#include<ParticleManager.h>
#include<Object3d.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI

class BaseEmitter {
public:
    //ほとんどのメンバ変数をコンストラクタの引数として受け取り、メンバ変数に代入する
    BaseEmitter(const std::string& name, const uint32_t count, const Transform& transform, const Vector4& color, float lifetime, float currentTime, const Velocity& Velocity, const RandomParameter& randomParameter)
        : name_(name), count_(count), transform_(transform), color_(color),
        frequency_(lifetime), frequencyTime_(currentTime), velocity_(Velocity),
        random_(randomParameter) {
    }

    virtual ~BaseEmitter() = default;
    virtual void Update() = 0;   // ← 動作ごとに違う
    void Emit() {
        auto& group = ParticleManager::GetInstance()->GetGroup(name_);

        // 現在生存中のパーティクル数
        uint32_t aliveCount = static_cast<uint32_t>(group.particles.size());

        // 残り発生可能数
        uint32_t remaining = maxParticles_ > aliveCount ? maxParticles_ - aliveCount : 0;
        if (remaining == 0) return;

        // 1回の発生数は残りを超えないように
        uint32_t emitCount = std::min(emitPerTick_, remaining);

        ParticleManager::GetInstance()->Emit(name_, transform_, color_, emitCount, velocity_, frequency_, random_);  
    }

    void SetTarget(Object3d* obj) { target_ = obj; } // FollowEmitter用 getter/setter


    // ImGui
    virtual void DrawImGuiUI() {
#ifdef USE_IMGUI
        if (ImGui::CollapsingHeader(name_.c_str())) {
            ImGui::DragInt("Count", reinterpret_cast<int*>(&count_), 1, 1, 1000);

            // Transform 調整
            ImGui::Text("Transform");
            ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
            ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.1f);
            ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);

            // Velocity 調整
            ImGui::Text("Velocity");
            ImGui::DragFloat3("Velocity Translate", &velocity_.translate.x, 0.01f);
            ImGui::DragFloat3("Velocity Rotate", &velocity_.rotate.x, 0.01f);
            ImGui::DragFloat3("Velocity Scale", &velocity_.scale.x, 0.01f);
            // Color
            ImGui::ColorEdit4("Color", &color_.x);

            // 発生周期(Frequency)の調整
            ImGui::DragFloat("Frequency (sec)", &frequency_, 0.01f, 0.0f, 60.0f);
            // --- 初期タイマー値 ---
            ImGui::DragFloat("Initial Elapsed Time", &frequencyTime_, 0.01f, 0.0f, 60.0f);            
            random_.DrawImGuiUI(); // RandomParameter の UI

            // ランダムパラメータのUI
            random_.DrawImGuiUI();

            // 連続発生関連
            ImGui::Separator();
            ImGui::Text("Continuous Emission Settings");
            ImGui::DragInt("Max Particles", reinterpret_cast<int*>(&maxParticles_), 1, 1, 10000);
            ImGui::DragInt("Emit Per Tick", reinterpret_cast<int*>(&emitPerTick_), 1, 1, 100);

            // 値の制限チェック（最大 < 最小にならないように）
            if (emitPerTick_ > static_cast<int>(maxParticles_)) emitPerTick_ = maxParticles_;
        }
#endif // USE_IMGUI
    }

protected:
    std::string name_;
    uint32_t count_;
    Transform transform_;
    Vector4 color_;
    float frequency_;
    float frequencyTime_;
    Velocity velocity_;
    RandomParameter random_;

    Object3d* target_ = nullptr; // ← FollowEmitter用
   
    bool useGravity_ = false;

public:
    uint32_t maxParticles_ = 100;   // 最大生成数
    uint32_t emitPerTick_ = 1;      // 1回のEmitで出す数
    uint32_t currentParticles_ = 0; // 現在生成済みパーティクル数

  
    void SetUseGravity(bool enable) { useGravity_ = enable; }

};