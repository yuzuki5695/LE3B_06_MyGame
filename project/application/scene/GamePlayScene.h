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
    /// 敵出現処理（レベルデータに基づく）
    /// </summary>
    void EnemySpawn();
    /// <summary>
    /// OBB同士の当たり判定
    /// </summary>
    bool IsOBBIntersect(const OBB& a, const OBB& b);
    /// <summary>
    /// プレイヤーの弾と敵との衝突判定処理
    /// </summary>
    void CheckBulletEnemyCollisionsOBB();
    /// <summary>
    /// 敵の弾とプレイヤーとの衝突判定処理
    /// </summary>
    void CheckEnemyBulletPlayerCollisionsOBB();
    /// <summary>
    /// V字フォーメーションでの敵出現処理
    /// </summary>
    void SpawnVFormation(const EnemySpawnTrigger& trigger);
    /// <summary>
    /// 逆ステップフォーメーションでの敵出現処理
    /// </summary>
    void SpawnReverseStepFormation(const EnemySpawnTrigger& trigger);
    /// <summary>
    /// ジグザグフォーメーションでの敵出現処理
    /// </summary>
    void SpawnZigZagFormation(const EnemySpawnTrigger& trigger);
private: // メンバ変数
    // オブジェクトデータ
    // 地面
    std::unique_ptr <Object3d> grass = nullptr; 
    // プレイヤー
	std::unique_ptr <Player> player_ = nullptr;
    bool playerhp_ = true;
    Transform transform_;
	// キャラクターローダー
    std::unique_ptr<CharacterLoader> levelLoader_ = nullptr;
    // レベルデータ格納用インスタンスを生成
    LevelData* levelData = nullptr;
    bool end = false;
    // 最大数
    int MAX_ENEMY;
	// 敵リスト
    std::vector<std::unique_ptr<Enemy>> enemies_;
	// 敵出現トリガーリスト
    std::vector<EnemySpawnTrigger> spawnTriggers_;
	// クリアゲート(仮)
    std::unique_ptr <Object3d> wall = nullptr;
    // ゴール判定用Z座標
	float goalpos_ = 300.0f;
	// ゴールフラグ
    bool goal_ = false;
	// スカイボックス
    std::unique_ptr <Skybox> Box_ = nullptr;
	// イベント処理
    std::vector<EventManager> event_; 
    /// UI(タイトルへ(仮))
    std::unique_ptr <Sprite> ui1_;
};