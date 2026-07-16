#pragma once
#include <Player.h>
#include <CameraManager.h>
#include <Enemy.h>
#include <random>

namespace MyGame {
    /// <summary>
	/// 敵のスポーンを管理するクラス
    /// </summary>
    class EnemySpawner {
    public: // メンバ関数
        /// <summary>
		/// 初期化処理 
		/// </summary>
        void Initialize();
        /// <summary>
        /// 更新処理
        /// </summary>
        void Update();
        /// <summary>
        /// 敵のスポーン処理
        /// </summary>
        /// <param name="count"></param>
        void SpawnEnemies(int count);
	private: // メンバ変数
        std::vector<std::unique_ptr<Enemy>>* enemies_ = nullptr;  // 敵リストへの参照
        Player* player_ = nullptr;                                // プレイヤ―の参照
        float spawnInterval_;        // 何秒ごと
        float spawnTimer_;           // スポーン経過時間
        int spawnCount_;             // 1回で出す数
        std::mt19937 randomEngine;   // 乱数生成器
        bool isEventLocked_ = false; // イベント中のスポーン停止フラグ
    public:
        void SetEnemies(std::vector<std::unique_ptr<Enemy>>* enemies) { enemies_ = enemies; }
        void SetPlayer(Player* player) { player_ = player; }
        void SetEventLocked(bool flag) { isEventLocked_ = flag; }
    };
}