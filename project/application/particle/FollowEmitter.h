#pragma once
#include<BaseEmitter.h>
#include<CameraManager.h>

class FollowEmitter : public BaseEmitter {
public:
    using BaseEmitter::BaseEmitter;

        
    Vector3 offset_{ 0.0f, 0.0f, 0.0f }; // ターゲットとの相対オフセット 
    // 初期化フラグ
    bool initialized_ = false;
    float emitInterval_ = 0.06f;         // 発生間隔（秒）

    void Update() override {
        if (target_) {
            transform_.translate = target_->GetTransform().translate + offset_;
            // カメラの向きに合わせて回転
            GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
            if (gameCam) {
                transform_.rotate = gameCam->GetActiveCamera()->GetRotate();
            }
        }

        // 発生タイマー更新
        frequencyTime_ += 1.0f / 60.0f;

        // 発生
        if (frequencyTime_ >= emitInterval_) {
            // Emit 直前に transform を確定
            if (target_) {
                transform_.translate = target_->GetTransform().translate + offset_;
            }
            Emit();
            frequencyTime_ = 0.0f;
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
            // オフセットを調整可能に
            ImGui::DragFloat3("Offset", &offset_.x, 0.1f); 
            // 発生間隔を調整可能に
            ImGui::DragFloat("Emit Interval (sec)", &emitInterval_, 0.01f, 0.0f, 60.0f);
        }
#endif // USE_IMGUI
    }



    // FollowEmitter.h
    void SetOffset(const Vector3& offset) { offset_ = offset; }
    Vector3 GetOffset() const { return offset_; }

    void SetVelocity(const Velocity& v) { velocity_ = v; }
    Velocity GetVelocity() const { return velocity_; }

};