#pragma once
#include <BaseScene.h>
#include <ParticleEmitter.h>
#include <SoundPlayer.h>
#include <Skybox.h>
#include <CharacterLoader.h>
#include <Player.h>
#include <Enemy.h>
#include <FadeManager.h>
#include <EventManager.h>
#include <GamePlayparticle.h>
#include <EnemySpawner.h>
#include <Pausemenu.h>

/// <summary>
/// 実際のゲームプレイを行うシーン。
/// プレイヤー操作、ステージ進行、イベント管理、
/// ポーズメニュー、UI表示などゲーム本編の処理を担当
/// </summary>
class GamePlayScene : public BaseScene {
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
    /// <summary>
    /// ポーズメニューの更新処理
    /// TABキー入力によるポーズ開始やメニュー操作を管理
    /// </summary>
    void PauseMenuUpdate();
    /// <summary>
    /// ポーズ中のコマンド処理。
    /// GoToTitle などの選択肢に応じた処理を実行。
    /// </summary>
    void HandlePauseMenuCommands();
    /// <summary>
    /// フェードイン・フェードアウト処理の管理。
    /// フェード開始、更新、終了チェックを行う。
    /// </summary>
    void HandleFadeInOut();
    /// <summary>
    /// プレイヤー状態ごとの処理。
    /// 死亡、ゴール、操作可能状態への遷移などを管理する。
    /// </summary>
    void HandlePlayerState();


    
    /// <summary>
    /// プレイヤーの弾と敵との衝突判定処理
    /// </summary>
    void CheckBulletEnemyCollisionsOBB();
    /// <summary>
    /// 敵の弾とプレイヤーとの衝突判定処理
    /// </summary>
    void CheckEnemyBulletPlayerCollisionsOBB();

    void CheckEnemyPlayerCollisionsOBB();
private: // メンバ変数
    // オブジェクトデータ
    // プレイヤー
    std::unique_ptr <Player> player_;
	bool end; /// ゲーム終了フラグ

    std::unique_ptr <Object3d> wall;    /// クリアゲート(仮)
    // ゴールフラグ
    bool goal_ = false;
    // スカイボックス
    std::unique_ptr <Skybox> Box_;
    std::vector<EventManager> event_;  /// イベント処理

	std::unique_ptr <GamePlayparticle> particles_; /// プレイ中のパーティクル

    std::unique_ptr <Pausemenu> pausemenu_;    /// ポーズメニュー
    bool isPausedevent_;                       /// ポーズ可能状態かどうか
    
    // 最大数
    int MAX_ENEMY;
    // 敵リスト
    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::unique_ptr<EnemySpawner> enemySpawner_;
    // 敵出現トリガーリスト
    std::vector<EnemySpawnTrigger> spawnTriggers_;
};