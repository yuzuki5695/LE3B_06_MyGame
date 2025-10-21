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

// ゲームプレイシーン
class GamePlayScene : public BaseScene
{
public: // メンバ関数
    // 初期化
    void Initialize() override;
    // 終了
    void Finalize() override;
    // 毎フレーム更新
    void Update() override;
    // 描画
    void Draw() override;
	// 敵出現処理
    void EnemySpawn();
	bool IsOBBIntersect(const OBB& a, const OBB& b); // OBB同士の当たり判定
    void CheckBulletEnemyCollisionsOBB();    // プレイヤーの弾と敵
    void CheckEnemyBulletPlayerCollisionsOBB();    // プレイヤーと敵の弾
	void SpawnVFormation(const EnemySpawnTrigger& trigger); // V字フォーメーション
	void SpawnReverseStepFormation(const EnemySpawnTrigger& trigger); // 逆ステップフォーメーション
	void SpawnZigZagFormation(const EnemySpawnTrigger& trigger); // ジグザグフォーメーション
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
};