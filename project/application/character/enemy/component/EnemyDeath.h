#pragma once
#include <Object3d.h>
#include <random>

namespace MyGame {
    /// <summary>
    /// 敵の死亡演出を管理するクラス
    /// </summary>
    class EnemyDeath {
    public: // メンバ関数
        /// <summary>
        /// 死亡演出を開始する
        /// </summary>
        void Start();
        /// <summary>
        /// 更新処理
        /// </summary>
        /// <param name="object">敵オブジェクト</param>
        void Update(MyEngine::Object3d* object);
    private: // メンバ変数
        bool isPlaying_;   // 死亡演出再生中フラグ
        bool isfinished_;  // 死亡演出終了フラグ
        float timer_;      // 演出経過時間
        float duration_;   // 演出時間
        std::mt19937 randomEngine_{ std::random_device{}() };// 乱数生成器
    public:
        bool IsFinished() const { return isfinished_; }
    };
}