#pragma once
#include <Transform.h>
#include <ParticleManager.h>
#include <MatrixVector.h>

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
        MyEngine::Transform& GetTransform() { return spawnData_.transform; } 
        MyEngine::Vector3& GetTranslate() { return spawnData_.transform.translate; }

        virtual void SetTranslate(MyEngine::Vector3 translate) { spawnData_.transform.translate = translate; }
        virtual void SetDirection(const MyEngine::Vector3& dir) { spawnData_.velocity.translate = MyEngine::MatrixVector::Normalize(dir); }
    };
}