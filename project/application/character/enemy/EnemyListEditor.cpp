#include "EnemyListEditor.h"
#include <Enemy.h>
#include <LineRenderer.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif

using namespace MyEngine;

namespace MyGame {

    // 静的メンバ変数の定義
    std::unique_ptr<EnemyListEditor> EnemyListEditor::instance = nullptr;

    // シングルトンインスタンスの取得
    EnemyListEditor* EnemyListEditor::GetInstance() {
        if (!instance) {
            instance = std::make_unique<EnemyListEditor>();
        }
        return instance.get();
    }

    // 終了
    void EnemyListEditor::Finalize() {
        instance.reset();  // `delete` 不要
    }

    void EnemyListEditor::DrawImGui() {
#ifdef USE_IMGUI

        if (!enemies_) {
            return;
        }

        ImGui::Begin("Enemy Manager");
        ImGui::Text("Enemy Count : %d", static_cast<int>(enemies_->size()));
        ImGui::Separator();

        // -----------------------------
        // 左：敵一覧
        // -----------------------------
        ImGui::BeginChild("EnemyList", ImVec2(250, 400), true);

        for (size_t i = 0; i < enemies_->size(); ++i) {

            Enemy* enemy = (*enemies_)[i].get();

            std::string label = "Enemy [" + std::to_string(i) + "]";

            if (!enemy->IsAlive()) {
                label += " [Dead]";
            }

            if (ImGui::Selectable(label.c_str(), selectedEnemy_ == static_cast<int>(i))) {
                selectedEnemy_ = static_cast<int>(i);
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();

        // -----------------------------
        // 右：詳細表示
        // -----------------------------
        ImGui::BeginChild("EnemyDetail", ImVec2(0, 400), true);

        if (selectedEnemy_ >= 0 &&
            selectedEnemy_ < enemies_->size()) {
            Enemy* enemy = (*enemies_)[selectedEnemy_].get();

            auto* object = enemy->GetObject3d();

            if (object) {
                object->DrawImGui("Enemy");
            }

            ImGui::Separator();

            ImGui::Text("Alive : %s",
                enemy->IsAlive() ? "True" : "False");

            ImGui::Text("Spawned : %s",
                enemy->IsSpawned() ? "True" : "False");

            ImGui::Text("Exp Reward : %u",
                enemy->GetExpReward());

            //ImGui::Checkbox("KilledByPlayer", &enemy->GetKilledByPlayer());

            //// Colliderサイズ調整
            //LineRenderer::GetInstance()->DrawImGui(enemy->GetColliderSize());
        }

        ImGui::EndChild();

        ImGui::End();

#endif
    }
}