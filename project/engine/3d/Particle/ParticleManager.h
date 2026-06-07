#pragma once
#include <DirectXCommon.h>
#include <SrvManager.h>
#include <random>
#include <Vector2.h>
#include <Model.h>
#include <ParticleModel.h>
#include <ParticleData.h>
#include <ParticleCommon.h>

namespace MyEngine {
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
		void Initialize(DirectXCommon* birectxcommon, SrvManager* srvmanager, ParticleCommon* particleCommon);
		// 更新処理
		void Update();
		// 描画処理
		void Draw();
		// パーティクルグループの作成
		void CreateParticleGroup(const std::string& name, const std::string& textureFilepath, const std::string& filename);
		// 発生
		void Emit(const std::string& name, const ParticleSpawnData& spawnData);

		void ClearAll();
	private: // 内部関数
		void CameraForGPUGenerate();
		void ParticleInfoBufferGenerate();  // ★追加：パーティクル基本情報のバッファ生成
		void SpawnListBufferGenerate();     // ★追加：全グループ共通のスポーン要求バッファ生成
		void GroupSpawnCBufferGenerate();   // ★追加：グループごとの範囲伝達用定数バッファ生成
		void ProcessSpawnRequests();
		void TransitionParticleBuffer(ParticleGroup& group, D3D12_RESOURCE_STATES after);
		void UAVBarrier(ID3D12Resource* resource);
	private: // メンバ変数
		// ポインタ
		DirectXCommon* dxCommon_;
		SrvManager* srvmanager_;
		ParticleCommon* particleCommon_;
		// ランダムエンジン
		std::mt19937 randomEngine;
		//最大インスタンス
		uint32_t MaxInstanceCount = 1500;
		//ビルボード行列
		Matrix4x4 backToFrontMatrix;
		// パーティクルグループコンテナ
		std::unordered_map<std::string, ParticleGroup> particleGroups;

		Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> particleInfoResource_;
		Microsoft::WRL::ComPtr<ID3D12Resource> spawnListResource_;
		Microsoft::WRL::ComPtr<ID3D12Resource> groupSpawnCBResource_;
		CameraData* cameraData = nullptr;
		ParticleInfo* particleInfoData_ = nullptr;
		SpawnRequestGPU* spawnListData_ = nullptr;
		GroupSpawnCB* groupSpawnCBData_ = nullptr;
	
		std::vector<SpawnRequest> spawnRequests_;
	public: // アクセッサ
		// getter
		ParticleGroup& GetGroup(const std::string& name) {
			assert(particleGroups.count(name));
			return particleGroups[name];
		}
		uint32_t GetMaxInstanceCount() const { return MaxInstanceCount; }

	};
}