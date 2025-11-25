#pragma once
#include<BaseEmitter.h>

class FollowEmitter : public BaseEmitter {
public:
    using BaseEmitter::BaseEmitter;

    void Update() override {
        // ターゲットがあれば位置を同期
        if (target_) {
            transform_.translate = target_->GetTransform().translate;
        }

        // 発生周期制御
        frequencyTime_ += 1.0f / 60.0f;
        if (frequencyTime_ >= frequency_) {
            frequencyTime_ = 0.0f;
            Emit();
        }
    }

    void DrawImGuiUI() override {
        BaseEmitter::DrawImGuiUI(); // 基本パラメータは共通 UI
#ifdef USE_IMGUI
        if (ImGui::CollapsingHeader("FollowEmitter")) {
            if (target_) {
                Vector3 pos = target_->GetTransform().translate;
                ImGui::Text("Following target at %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
            } else {
                ImGui::Text("No target assigned");
            }
        }
#endif // USE_IMGUI
    }
};