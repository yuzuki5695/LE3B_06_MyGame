#pragma once
#include<DirectXCommon.h>
#include<SrvManager.h>
#include<random>
#include<Vector2.h>
#include<Model.h>
#include<ParticleModel.h>
#include<RandomParameter.h>
#include<ParticleRandomData.h>
#include<ParticleGroup.h>

// 3Dオブジェクト共通部
class ParticleManager {
public:
	static std::unique_ptr<ParticleManager> instance;

	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;
public: // メンバ関数
	ParticleManager() = default;
	~ParticleManager() = default;

	// シングルトンインスタンスの取得
	static ParticleManager* GetInstance();
	// 終了
	void Finalize();
	// 初期化
	void Initialize(DirectXCommon* birectxcommon, SrvManager* srvmanager);
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	// パーティクルグループの作成
	void CreateParticleGroup(const std::string& name, const std::string& textureFilepath, const std::string& filename, VertexType vertexType);

	// 発生
	void Emit(const std::string& name, const Transform& transform, const Vector4& color, uint32_t count, const Velocity& velocity, float lifetime, const RandomParameter& randomParameter);

	void SetParticleGroupTexture(const std::string& name, const std::string& textureFilepath);
	void SetParticleGroupModel(const std::string& name, const std::string& modelFilepath);

	void DebugUpdata();

	ParticleRandomData GenerateRandomParticleData(const RandomParameter& param, const Velocity& baseVelocity, float baseLifetime, std::mt19937& randomEngine);
	
	void ClearAll();

private: // メンバ変数
	// ポインタ
	DirectXCommon* dxCommon_;
	SrvManager* srvmanager_;
	// ランダムエンジン
	std::mt19937 randomEngine;
	//最大インスタンス
	uint32_t MaxInstanceCount = 200;
	//ビルボード行列
	Matrix4x4 backToFrontMatrix;
	// パーティクルグループコンテナ
	std::unordered_map<std::string, ParticleGroup> particleGroups;

public:
	// getter
	ParticleGroup& GetGroup(const std::string& name) {
		assert(particleGroups.count(name));
		return particleGroups[name];
	}
	uint32_t GetMaxInstanceCount() const { return MaxInstanceCount; }

};