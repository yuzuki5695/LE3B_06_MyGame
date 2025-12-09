#pragma once
#include <Vector4.h>
#include <Velocity.h>
#ifdef USE_IMGUI
#include<ImGuiManager.h>
#endif // USE_IMGUI
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

struct RandomParameter {
    // ランダムな速度の範囲
    Vector3 offsetMin{ 0,0,0 };
    Vector3 offsetMax{ 0,0,0 };
    // ランダムな回転の範囲
    Vector3 rotateMin{ 0,0,0 };
    Vector3 rotateMax{ 0,0,0 };
    // ランダムなスケールの範囲
    Vector3 scaleMin{ 0,0,0 };
    Vector3 scaleMax{ 0,0,0 };
    // ランダムな色の範囲
    float colorMin = 0.0f; // 最小値
    float colorMax = 0.0f; // 最大値
    // ランダムな寿命の範囲を追加
    float lifetimeMin = 0.0f;
    float lifetimeMax = 0.0f;
    // ランダムな速度の範囲を追加
    Velocity velocityMin{};
    Velocity velocityMax{};

    // ---------- setter ----------
    RandomParameter& SetOffset(const Vector3& min, const Vector3& max) {
        offsetMin = min; offsetMax = max;
        return *this;
    }
    RandomParameter& SetRotate(const Vector3& min, const Vector3& max) {
        rotateMin = min; rotateMax = max;
        return *this;
    }
    RandomParameter& SetScale(const Vector3& min, const Vector3& max) {
        scaleMin = min; scaleMax = max;
        return *this;
    }
    RandomParameter& SetColor(float min, float max) {
        colorMin = min; colorMax = max;
        return *this;
    }
    RandomParameter& SetLifetime(float min, float max) {
        lifetimeMin = min; lifetimeMax = max;
        return *this;
    }
    RandomParameter& SetVelocity(const Velocity& min, const Velocity& max) {
        velocityMin = min; velocityMax = max;
        return *this;
    }

    // --- 便利：軸ごとに設定するバージョン（必要なら追加） ---
    RandomParameter& SetRotateZ(float min, float max) {
        rotateMin.z = min; rotateMax.z = max;
        return *this;
    }


    // setters
    void DrawImGuiUI() {
#ifdef USE_IMGUI
        // Offset
        if (ImGui::TreeNode("Offset")) {
            ImGui::DragFloat3("Min", &offsetMin.x, 0.01f);
            ImGui::DragFloat3("Max", &offsetMax.x, 0.01f);
            // Min <= Max 制御
            offsetMin.x = std::min(offsetMin.x, offsetMax.x);
            offsetMin.y = std::min(offsetMin.y, offsetMax.y);
            offsetMin.z = std::min(offsetMin.z, offsetMax.z);
            offsetMax.x = std::max(offsetMax.x, offsetMin.x);
            offsetMax.y = std::max(offsetMax.y, offsetMin.y);
            offsetMax.z = std::max(offsetMax.z, offsetMin.z);
            ImGui::TreePop();
        }

        // Rotation
        if (ImGui::TreeNode("Rotation")) {
            ImGui::DragFloat3("Min", &rotateMin.x, 0.01f);
            ImGui::DragFloat3("Max", &rotateMax.x, 0.01f);
            rotateMin.x = std::min(rotateMin.x, rotateMax.x);
            rotateMin.y = std::min(rotateMin.y, rotateMax.y);
            rotateMin.z = std::min(rotateMin.z, rotateMax.z);
            rotateMax.x = std::max(rotateMax.x, rotateMin.x);
            rotateMax.y = std::max(rotateMax.y, rotateMin.y);
            rotateMax.z = std::max(rotateMax.z, rotateMin.z);
            ImGui::TreePop();
        }

        // Scale
        if (ImGui::TreeNode("Scale")) {
            ImGui::DragFloat3("Min", &scaleMin.x, 0.01f);
            ImGui::DragFloat3("Max", &scaleMax.x, 0.01f);
            scaleMin.x = std::min(scaleMin.x, scaleMax.x);
            scaleMin.y = std::min(scaleMin.y, scaleMax.y);
            scaleMin.z = std::min(scaleMin.z, scaleMax.z);
            scaleMax.x = std::max(scaleMax.x, scaleMin.x);
            scaleMax.y = std::max(scaleMax.y, scaleMin.y);
            scaleMax.z = std::max(scaleMax.z, scaleMin.z);
            ImGui::TreePop();
        }

        // Color
        if (ImGui::TreeNode("Color")) {
            ImGui::DragFloat("Min", &colorMin, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Max", &colorMax, 0.01f, 0.0f, 1.0f);
            colorMin = std::min(colorMin, colorMax);
            colorMax = std::max(colorMax, colorMin);
            ImGui::TreePop();
        }

        // Lifetime
        if (ImGui::TreeNode("Lifetime")) {
            ImGui::DragFloat("Min", &lifetimeMin, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Max", &lifetimeMax, 0.01f, 0.0f, 10.0f);
            lifetimeMin = std::min(lifetimeMin, lifetimeMax);
            lifetimeMax = std::max(lifetimeMax, lifetimeMin);
            ImGui::TreePop();
        }

        // Velocity
        if (ImGui::TreeNode("Velocity")) {
            ImGui::Text("Translate");
            ImGui::DragFloat3("Min Translate", &velocityMin.translate.x, 0.01f);
            ImGui::DragFloat3("Max Translate", &velocityMax.translate.x, 0.01f);
            velocityMin.translate.x = std::min(velocityMin.translate.x, velocityMax.translate.x);
            velocityMin.translate.y = std::min(velocityMin.translate.y, velocityMax.translate.y);
            velocityMin.translate.z = std::min(velocityMin.translate.z, velocityMax.translate.z);
            velocityMax.translate.x = std::max(velocityMax.translate.x, velocityMin.translate.x);
            velocityMax.translate.y = std::max(velocityMax.translate.y, velocityMin.translate.y);
            velocityMax.translate.z = std::max(velocityMax.translate.z, velocityMin.translate.z);

            ImGui::Text("Rotate");
            ImGui::DragFloat3("Min Rotate", &velocityMin.rotate.x, 0.01f);
            ImGui::DragFloat3("Max Rotate", &velocityMax.rotate.x, 0.01f);
            velocityMin.rotate.x = std::min(velocityMin.rotate.x, velocityMax.rotate.x);
            velocityMin.rotate.y = std::min(velocityMin.rotate.y, velocityMax.rotate.y);
            velocityMin.rotate.z = std::min(velocityMin.rotate.z, velocityMax.rotate.z);
            velocityMax.rotate.x = std::max(velocityMax.rotate.x, velocityMin.rotate.x);
            velocityMax.rotate.y = std::max(velocityMax.rotate.y, velocityMin.rotate.y);
            velocityMax.rotate.z = std::max(velocityMax.rotate.z, velocityMin.rotate.z);

            ImGui::Text("Scale");
            ImGui::DragFloat3("Min Scale", &velocityMin.scale.x, 0.01f);
            ImGui::DragFloat3("Max Scale", &velocityMax.scale.x, 0.01f);
            velocityMin.scale.x = std::min(velocityMin.scale.x, velocityMax.scale.x);
            velocityMin.scale.y = std::min(velocityMin.scale.y, velocityMax.scale.y);
            velocityMin.scale.z = std::min(velocityMin.scale.z, velocityMax.scale.z);
            velocityMax.scale.x = std::max(velocityMax.scale.x, velocityMin.scale.x);
            velocityMax.scale.y = std::max(velocityMax.scale.y, velocityMin.scale.y);
            velocityMax.scale.z = std::max(velocityMax.scale.z, velocityMin.scale.z);

            ImGui::TreePop();
        }

#endif // USE_IMGUI
    }
};