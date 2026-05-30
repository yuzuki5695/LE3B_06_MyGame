#pragma once
#include <vector>
#include <memory>

namespace MyGame {

    class Enemy;

    class EnemyListEditor {
    private:
        static std::unique_ptr<EnemyListEditor> instance;

        EnemyListEditor(EnemyListEditor&) = delete;
        EnemyListEditor& operator=(EnemyListEditor&) = delete;
    public: // メンバ関数
        EnemyListEditor() = default;
        ~EnemyListEditor() = default;
        // シングルトンインスタンスの取得
        static EnemyListEditor* GetInstance();
        // 終了
        void Finalize();

        void DrawImGui();

    private:
        std::vector<std::unique_ptr<Enemy>>* enemies_ = nullptr;
        int selectedEnemy_ = -1;

    public:
        void SetEnemies(std::vector<std::unique_ptr<Enemy>>* enemies) { enemies_ = enemies; }
    };
}