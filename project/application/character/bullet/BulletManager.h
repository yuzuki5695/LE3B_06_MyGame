#pragma once
#include <vector>
#include <BaseBullet.h>

namespace MyGame {

    class BulletManager {
    private: // シングルトンインスタンス
        static BulletManager* instance;
        // コピーコンストラクタを防ぐ
        BulletManager(BulletManager&) = delete;
        BulletManager& operator=(BulletManager&) = delete;
    public: // メンバ関数 
        BulletManager() = default;
        ~BulletManager() = default;
        // シングルトンインスタンスの取得
        static BulletManager* GetInstance();
        /// <summary> 
        /// 終了処理  
        /// </summary> 
        void Finalize();

        void Update();
        void Draw();

        // プレイヤー弾生成
        void SpawnPlayerBullet(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity);
        // 敵弾生成
        void SpawnEnemyBullet(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity);

    private:
        std::vector<std::unique_ptr<BaseBullet>> bullets_;
    };
}