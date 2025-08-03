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

    void EnemySpawn();

    bool IsOBBIntersect(const OBB& a, const OBB& b);
    // プレイヤーの弾と敵
    void CheckBulletEnemyCollisionsOBB();

private: // メンバ変数
    // オブジェクトデータ
    // 地面
    std::unique_ptr <Object3d> grass = nullptr;
   
	std::unique_ptr <Player> player_ = nullptr;
    
    Transform transform_;

	// キャラクターローダー
    std::unique_ptr<CharacterLoader> levelLoader_ = nullptr;
    // レベルデータ格納用インスタンスを生成
    LevelData* levelData = nullptr;
    bool end = false;
        
    int MAX_ENEMY = 6;

    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::vector<EnemySpawnTrigger> spawnTriggers_;


        
    std::unique_ptr <Object3d> clear = nullptr;
           
    std::unique_ptr <Object3d> wall = nullptr;
    
    std::unique_ptr <Sprite> black = nullptr;
};