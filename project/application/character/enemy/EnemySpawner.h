#pragma once
#include <Player.h>
#include <CameraManager.h>
#include <Enemy.h>
#include <random>

namespace MyGame {

    // 前方宣言
    //class Player;

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
        float spawnInterval_ = 3.0f;   // 何秒ごと
        float spawnTimer_ = 0.0f;
        int spawnCount_ = 3;           // 1回で出す数
        std::vector<std::unique_ptr<Enemy>>* enemies_ = nullptr;

        std::mt19937 randomEngine;               // 乱数生成器

        Player* player_ = nullptr;
        bool isEventLocked_ = false;
    public:
        void SetEnemies(std::vector<std::unique_ptr<Enemy>>* enemies) { enemies_ = enemies; }
        void SetPlayer(Player* player) { player_ = player; }
        void SetEventLocked(bool flag) { isEventLocked_ = flag; }
    };
}