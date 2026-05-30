#pragma once
#include <vector>
#include <memory>
#include <Vector3.h>

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
        /// <summary>
        /// 初期化処理 
        /// </summary>
        void Initialize();

    private:
        std::vector<std::unique_ptr<Enemy>>* enemies_ = nullptr;
        int selectedEnemy_ = -1;
        // ======================================================== 
        // すべての敵に一括反映させるための共通パラメータ用変数        
        // ========================================================
        float commonScale_;       // 全敵の共通スケール値
        MyEngine::Vector3 commonColliderSize_;
        int commonExpReward_;       // 全敵の共通獲得経験値
    public:
        void SetEnemies(std::vector<std::unique_ptr<Enemy>>* enemies) { enemies_ = enemies; }
    };
}