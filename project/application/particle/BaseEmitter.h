#pragma once
#include <Transform.h>
#include <ParticleManager.h>

namespace MyGame {

    class BaseEmitter {
    public: // メンバ関数

        virtual ~BaseEmitter() = default;

        BaseEmitter(const std::string& name, const MyEngine::ParticleSpawnData& spawnData, const MyEngine::ParticleSpawnRandom& random);

        // パーティクル発生
        virtual void Emit();

    protected:
        MyEngine::ParticleSpawnData BuildSpawnData(MyEngine::ParticleManager& pm);
    protected: // メンバ変数
        // 名前
        std::string name_;
        // 発生データ
        MyEngine::ParticleSpawnData spawnData_;
        // ランダムパラメータ
        MyEngine::ParticleSpawnRandom random_;
    public:
        MyEngine::ParticleSpawnData& GetSpawnData() { return spawnData_; }
      
        virtual void SetPosition(const MyEngine::Vector3& pos) { spawnData_.transform.translate = pos; }
    };
}