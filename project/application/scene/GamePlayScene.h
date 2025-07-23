#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include<Skybox.h>
#include<CharacterLoader.h>

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

private: // メンバ変数
    // オブジェクトデータ
    // 地面
    std::unique_ptr <Object3d> grass = nullptr;
   
    Transform transform_;

	// キャラクターローダー
    std::unique_ptr<CharacterLoader> levelLoader_ = nullptr;
    // レベルデータ格納用インスタンスを生成
    LevelData* levelData = nullptr;
    // オブジェクトコンテナ
    std::vector<std::unique_ptr<Object3d>> object3ds_;

};