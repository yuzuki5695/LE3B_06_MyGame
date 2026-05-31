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
        void Initialize();
        void Update();
        void Draw();
        void DrawImGui();

        // プレイヤー弾生成
        void SpawnPlayerBullet(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity);
        // 敵弾生成
        void SpawnEnemyBullet(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity);

    private:
        std::vector<std::unique_ptr<BaseBullet>> bullets_;
        // 共通のコライダーサイズパラメータ
        MyEngine::Vector3 commonPlayerBulletSize_ = { 1.0f, 1.0f, 1.0f };
        MyEngine::Vector3 commonEnemyBulletSize_ = { 1.0f, 1.0f, 1.0f };
    };
}