#pragma once
#include <ParticleManager.h>
#include <ParticleEmitter.h>
#include<FollowEmitter.h>
#include<DefaultParticleEmitter.h>

// タイトル専用パーティクル
class GamePlayparticle {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(Object3d* target);
    /// <summary>
    /// 毎フレームの更新処理
    /// </summary>
    void Update();
        // 敵ヒット時に呼ぶ（複数敵対応）
    void AddHitPosition(const Vector3& pos) { hitPositions_.push_back(pos); }


private: // メンバ変数
    // エミッター生成
    std::unique_ptr <FollowEmitter> particles_;
    // ランダムパラメータ
    RandomParameter random_;
        // エミッター生成
    std::unique_ptr <FollowEmitter> particles_02_;


  
    std::vector<std::unique_ptr<DefaultParticleEmitter>> explosionEmitters_;
    std::vector<std::unique_ptr<DefaultParticleEmitter>> shockwaveEmitters_;
    // ランダムパラメータ
    RandomParameter shockwaveErandom_;
    RandomParameter explosionErandom_;
    float time_ = 0.0f;
    bool exploded_ = false;
      
    bool hitEnemy_ = false;      // 敵に当たったら true
std::vector<Vector3> hitPositions_;
    bool move = false; 
public:

    void SethitEnemy_(bool hit) { hitEnemy_ = hit; }
};