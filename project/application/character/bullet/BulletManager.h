#pragma once
#include <vector>
#include <BaseBullet.h>

namespace MyGame {
    /// <summary>
    /// 弾の管理クラス
    /// </summary>
    class BulletManager {
    private: // シングルトンインスタンス
        static BulletManager* instance_;
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
        /// <summary> 
        /// 初期化処理  
        /// </summary> 
        void Initialize();
        /// <summary> 
        /// 更新処理  
        /// </summary> 
        void Update();
        /// <summary> 
        /// 描画処理  
        /// </summary> 
        void Draw();
        /// <summary> 
        /// Imgui処理  
        /// </summary> 
        void DrawImGui();      
        /// <summary> 
        /// プレイヤー弾生成  
        /// </summary> 
        /// <param name="transform"></param>
        /// <param name="velocity"></param>
        void SpawnPlayerBullet(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity);
        /// <summary> 
        /// 敵弾生成
        /// </summary> 
        /// <param name="transform"></param>
        /// <param name="velocity"></param>
        void SpawnEnemyBullet(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity);
    private:
        std::vector<std::unique_ptr<BaseBullet>> bullets_;
        // 共通のコライダーサイズパラメータ
        MyEngine::Vector3 commonPlayerBulletSize_ = { 1.0f, 1.0f, 1.0f };
        MyEngine::Vector3 commonEnemyBulletSize_ = { 1.0f, 1.0f, 1.0f };
    };
}