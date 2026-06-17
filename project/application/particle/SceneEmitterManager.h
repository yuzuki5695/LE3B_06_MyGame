#pragma once
#include <Transform.h>
#include <BaseEmitter.h>
#include <SceneParticleBase.h>

namespace MyGame {

    class SceneEmitterManager {
    private: // シングルトンインスタンス
        static std::unique_ptr<SceneEmitterManager> instance;

        // シングルトン設計のためコピーを禁止
        SceneEmitterManager(const SceneEmitterManager&) = delete;
        SceneEmitterManager& operator=(const SceneEmitterManager&) = delete;
    public:// メンバ関数
        SceneEmitterManager() = default;
        ~SceneEmitterManager() = default;
        /// <summary>
        /// シングルトンインスタンスの取得
        /// </summary>
        /// <returns>UIManagerのポインタ</returns>
        static SceneEmitterManager* GetInstance();
        /// <summary>
        /// インスタンスの破棄
        /// </summary>
        void Finalize();
        /// <summary>
        /// 現在のシーンに応じたパーティクルの再構築
        /// SceneManagerから現在のシーン名を取得し、古いパーティクルのを破棄して新しいパーティクルのを生成する。
        /// </summary>
        void Initialize();
        /// <summary>
        /// 更新処理
        /// リストに登録されている（＝現在のシーン用の）全パーティクルののUpdateを呼び出す。
        /// </summary>
        void Update();
        /// <summary>
        /// 内部リストへのパーティクルの追加
        /// </summary>
        /// <param name="sceneName">そのパーティクルのが所属するシーン名</param>
        /// <param name="パーティクルの">パーティクルのインスタンスの所有権</param>
        void AddUI(const std::string& sceneName, std::unique_ptr<SceneParticleBase> emitter);
        /// <summary>
        /// 管理している全UIの破棄
        /// </summary>
        void Clear();
    private: // メンバ変数
        // 現在のシーンで有効なUIインスタンスを保持するリスト
        std::vector<std::unique_ptr<SceneParticleBase>> emitterlist_;
    public:
        template <typename T>
        T* GetEmitter() {
            for (std::unique_ptr<SceneParticleBase>& emitter : emitterlist_) {
                T* target = dynamic_cast<T*>(emitter.get());
                if (target) {
                    return target;
                }
            }
            return nullptr;
        }
    };
}