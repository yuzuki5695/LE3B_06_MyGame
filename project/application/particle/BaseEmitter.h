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
    BaseEmitter(const std::string& name, const uint32_t count,
        const Transform& transform, const Vector4& color,
        float lifetime, float currentTime,
        const Velocity& Velocity, const RandomParameter& randomParameter)
        : name_(name), count_(count), transform_(transform), color_(color),
        frequency_(lifetime), frequencyTime_(currentTime), velocity_(Velocity),
        random_(randomParameter) {
    }

    virtual ~BaseEmitter() = default;
    virtual void Update() = 0;   // ← 動作ごとに違う
    void Emit() { ParticleManager::GetInstance()->Emit(name_, transform_, color_, count_, velocity_, frequency_, random_); }
    void SetTarget(Object3d* obj) { target_ = obj; } // FollowEmitter用 getter/setter

       
    // ImGui
    virtual void DrawImGuiUI() {
#ifdef USE_IMGUI
        if (ImGui::CollapsingHeader(name_.c_str())) {
            ImGui::DragInt("Count", reinterpret_cast<int*>(&count_), 1, 1, 1000);
            ImGui::DragFloat3("Position", &transform_.translate.x, 0.01f);
            ImGui::ColorEdit4("Color", &color_.x);
            ImGui::DragFloat("Frequency", &frequency_, 0.01f, 0.0f, 10.0f);
            random_.DrawImGuiUI(); // RandomParameter の UI
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
};