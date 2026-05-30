#include "EnemyListEditor.h"
#include <Enemy.h>
#include <LineRenderer.h>
#include <EditorEntityRegistry.h>
#include <EditorTypes.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI

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
    void EnemyListEditor::Initialize() {
#ifdef USE_IMGUI
        commonScale_ = 1.0f;       // 全敵の共通スケール値
        //commonExpReward_ = 10;       // 全敵の共通獲得経験値
        commonColliderSize_ = { 1.0f, 1.0f, 1.0f };        // 全敵の共通コライダーサイズ初期値

        EditorTypes::EditorObjectInfo info;
        info.name = "Enemy List";                  // Objectメニューに表示される名前
        info.category = EditorTypes::ObjectCategory::List; // 新設したカテゴリ（またはObject3Dなど既存のものでもOK）
        info.objectPtr = nullptr;                          // 単一オブジェクトではないためnullptr
        // ラムダ式に敵リストの情報を閉じ込める
        info.drawEditor = [this]() {
            if (!enemies_) {
                ImGui::Text("Enemy list is not set.");
                return;
            }
            // ========================================================             
            // 1. 基本ステータス 
            // ========================================================
            size_t totalEnemies = enemies_->size();
            size_t aliveCount = 0;
            size_t activeCount = 0;

            for (const auto& enemy : *enemies_) {
                if (enemy) {
                    if (enemy->IsAlive()) { aliveCount++; }
                    if (enemy->IsActive()) { activeCount++; }
                }
            }

            ImGui::SeparatorText("Enemy Pool Status");
            ImGui::Text("Total Pool Size : %zu", totalEnemies);
            ImGui::Text("Alive Enemies  : %zu", aliveCount);
            ImGui::Text("Active Enemies : %zu", activeCount);

            // ========================================================
            // 2. すべての敵に一括反映させる共通パラメータ設定
            // ========================================================
            ImGui::SeparatorText("All Enemy Common Parameters");

            // ① スケール値の編集 (元々のEnemy::DrawImGuiの変形要素に相当)
           // ImGui::DragFloat("Common Scale", &commonScale_, 0.05f, 0.1f, 10.0f, "%.2f");

            // ② LineRenderer/コライダーの基本パラメータ (Playerと同様の仕組み)
            ImGui::Text("Common Collider Settings");
            LineRenderer::GetInstance()->DrawImGui(commonColliderSize_);

            ImGui::Spacing();

            // ★このボタンを押すと、300体すべての敵のパラメータがまとめて書き換わる
            if (ImGui::Button("Apply parameters to ALL enemies", ImVec2(-1, 30))) {
                for (auto& enemy : *enemies_) {
                    if (!enemy) continue;

                    //// スケールの一括変更（Object3dを介して反映）
                    //if (enemy->GetObject3d()) {
                    //    enemy->GetObject3d()->SetScale({ commonScale_, commonScale_, commonScale_ });
                    //}

                    // コライダーサイズの一括変更
                    enemy->SetColliderSize(commonColliderSize_);
                }
            }

            // ========================================================
            // 3. リストの簡略表示
            // ========================================================
            ImGui::SeparatorText("Simple Enemy Status List");
            ImGui::BeginChild("EnemyListScroll", ImVec2(0, 150), true);

            for (size_t i = 0; i < enemies_->size(); ++i) {
                const auto& enemy = (*enemies_)[i];
                if (!enemy) continue;

                if (enemy->IsAlive()) {
                    ImGui::Text("[%03zu] ALIVE  | Active: %s", i, enemy->IsActive() ? "ON " : "OFF");
                } else {
                    ImGui::TextDisabled("[%03zu] DEAD", i);
                }
            }
            ImGui::EndChild();
            };
        // オブジェクト情報を登録する
        EditorEntityRegistry::Instance().Register(info);
#endif // USE_IMGUI
    }
}