#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include<Skybox.h>
#include<CharacterLoader.h>
#include<Player.h>
#include<Enemy.h>
#include<FadeManager.h>
#include<EventManager.h>
#include<StageManager.h>
#include<GamePlayparticle.h>
#include <EnemySpawner.h>

/// <summary>
/// ゲームプレイシーン
/// </summary>
class GamePlayScene : public BaseScene
{
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
    /// プレイヤーの弾と敵との衝突判定処理
    /// </summary>
    void CheckBulletEnemyCollisionsOBB();
    /// <summary>
    /// 敵の弾とプレイヤーとの衝突判定処理
    /// </summary>
    void CheckEnemyBulletPlayerCollisionsOBB();

    void CheckEnemyPlayerCollisionsOBB();

    void StartStageProgressUI();
    void UpdateStageProgressUI();
    void UpdateRandomMove();

private: // メンバ変数
    // オブジェクトデータ
    // プレイヤー
    std::unique_ptr <Player> player_ = nullptr;
    bool playerhp_ = true;
    // キャラクターローダー
    std::unique_ptr<CharacterLoader> levelLoader_ = nullptr;
    // レベルデータ格納用インスタンスを生成
    LevelData* levelData = nullptr;
    bool end;
    // 最大数
    int MAX_ENEMY;
    // 敵リスト
    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::unique_ptr<EnemySpawner> enemySpawner_;
    // 敵出現トリガーリスト
    std::vector<EnemySpawnTrigger> spawnTriggers_;
    // クリアゲート(仮)
    std::unique_ptr <Object3d> wall = nullptr;
    // ゴールフラグ
    bool goal_ = false;
    // スカイボックス
    std::unique_ptr <Skybox> Box_ = nullptr;
    // イベント処理
    std::vector<EventManager> event_;
    /// UI(タイトルへ(仮))
    std::unique_ptr <Sprite> ui1_;
    std::vector<std::unique_ptr<Sprite>> uis_;
    uint32_t MAXui_;

    std::unique_ptr <GamePlayparticle> particles_;

    std::unique_ptr <Sprite> gage_;
    std::unique_ptr <Sprite> player_ui_;
    // ===== UI進行管理 =====
    float uiStartRailLength_ = 0.0f;
    bool uiProgressStarted_ = false;
    bool uiProgressFinished_ = false;


};