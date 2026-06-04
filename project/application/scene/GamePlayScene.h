#pragma once
#include <BaseScene.h>
#include <Sprite.h>
#include <Player.h>
#include <Enemy.h>
#include <EnemySpawner.h>
#include <ParticleEmitter.h>

namespace MyGame {

    /// <summary>
    /// 実際のゲームプレイを行うシーン。
    /// </summary>
    class GamePlayScene : public MyEngine::BaseScene {
    public: // メンバ関数
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize() override;
        /// <summary>
        /// 終了処理
        /// </summary>
        void Finalize() override;
        /// <summary>
        /// 毎フレーム更新処理
        /// </summary>
        void Update() override;
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw() override;

    private: // 内部関数
        //// ゲーム開始イベントの更新処理
        //void StartEvent(); 
        // ゲーム終了イベントの更新処理
        void GameEnd();
    private: // メンバ変数
        std::unique_ptr <Player> player_;            // プレイヤー
        std::unique_ptr<EnemySpawner> enemySpawner_; // 敵スポーン管理クラス
        std::vector<std::unique_ptr<Enemy>> enemies_; // 敵キャラクターのリスト
        bool isGameStartEventDone_; //   ゲーム開始イベントが完了したかどうかのフラグ
        bool gamened_;
        uint32_t prevPlayerExp_; // 前フレームのプレイヤーの経験値
        bool isFirstExpGainIgnored_ = false; // 経験値の初期化が完了したかどうかのフラグ
        std::unique_ptr<ParticleEmitter> particleEmitter_;
    };
}