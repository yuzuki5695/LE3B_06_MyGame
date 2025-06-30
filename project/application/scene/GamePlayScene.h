#pragma once
#include<Sprite.h>
#include<Object3d.h>
#include<Model.h>
#include<SoundPlayer.h>
#include<BaseScene.h>
#include<ParticleEmitter.h>
#include<Player.h>
#include<Enemy.h>
#include<BulletManager.h>

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

    // 当たり判定(プレイヤーの球と敵)
    void CheckBulletEnemyCollisions();
    // 当たり判定(プレイヤーと敵)
    void CheckPlayerEnemyCollisions();
    // 当たり判定(プレイヤーと敵の弾)
    void CheckEnemyBulletPlayerCollisions();

    inline float Length(const Vector3& v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

private: // メンバ変数
    // オブジェクトデータ
    // 地面
    std::unique_ptr <Object3d> grass = nullptr;

    // 音声ファイル
    SoundData soundData;
    // 音声プレイフラグ
    uint32_t soundfige;

    Player* playerPtr;
 
    int MAXEnemy = 21;

    std::vector<EnemySpawnTrigger> spawnTriggers = {
    {50.0f, 3, false},
    {200.0f, 3, false},
    {350.0f, 3, false},
    {500.0f, 3, false},
    {600.0f, 3, false},        
    {700.0f, 3, false},
    {800.0f, 3, false},
    };

};