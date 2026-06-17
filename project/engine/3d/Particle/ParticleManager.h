#pragma once
#include <DirectXCommon.h>
#include <SrvManager.h>
#include <random>
#include <Vector2.h>
#include <Model.h>
#include <ParticleModel.h>
#include <ParticleData.h>
#include <ParticleCommon.h>
#include <MappedBuffer.h>

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
		void CreateParticleGroup(const std::string& name, const std::string& textureFilepath, const std::string& filename, const uint32_t& MaxInstanceCount);
		// 発生
		void Emit(const std::string& name, const ParticleSpawnData& spawnData);

		void ClearAll();
	private: // 内部関数
		// ProcessSpawnRequests から分割した段階的な関数群
		struct ActiveGroupSpawn { ParticleGroup* group; uint32_t startIndex; uint32_t count; };
		// 要求を1つのリニアな配列（CommandQueue）に詰め込み、各グループの生成範囲を特定する (CPU処理)
		uint32_t BuildSpawnRequests(std::vector<ActiveGroupSpawn>& outActiveSpawns);
		// 構築されたリクエストを基に、実際にComputeShaderのDispatchを実行する (GPU処理)
		void DispatchSpawnCommands(const std::vector<ActiveGroupSpawn>& activeSpawns);
		// リソース遷移・バリア関連（共通化）
		void TransitionParticleBuffer(ParticleGroup& group, D3D12_RESOURCE_STATES after);
		void TransitionParticleState(ParticleGroup& group, D3D12_RESOURCE_STATES after);
		void TransitionToDrawState(ParticleGroup& group);
		void PipelineUAVBarriers(const std::vector<ID3D12Resource*>& resources);
	private: // メンバ変数
		// ポインタ
		DirectXCommon* dxCommon_;
		SrvManager* srvmanager_;
		ParticleCommon* particleCommon_;
		// ランダムエンジン
		std::mt19937 randomEngine;
		//ビルボード行列
		Matrix4x4 backToFrontMatrix;
		// パーティクルグループコンテナ
		std::unordered_map<std::string, ParticleGroup> particleGroups;

		MappedBuffer<CameraData> cameraBuffer_;
		MappedBuffer<ParticleInfo> particleInfoBuffer_;
		MappedBuffer<SpawnRequestGPU> spawnListBuffer_;
		MappedBuffer<GroupSpawnCB> groupSpawnBuffer_;
		MappedBuffer<AttractInfo> attractInfoBuffer_;

		Vector3 attractTargetPosition_ = { 0.0f,0.0f,0.0f };
		std::vector<SpawnRequest> spawnRequests_;
	public: // アクセッサ
		// getter
		ParticleGroup& GetGroup(const std::string& name) {
			assert(particleGroups.count(name));
			return particleGroups[name];
		}

		float Rand(float minVal, float maxVal) { // 💡マクロとの衝突を防ぐため引数名変更
			std::uniform_real_distribution<float> dist(minVal, maxVal);
			return dist(randomEngine);
		}
		Vector3 RandVec3(const RandomRange<Vector3>& r) { return { this->Rand(r.min.x, r.max.x), this->Rand(r.min.y, r.max.y), this->Rand(r.min.z, r.max.z) }; }
		float SafeRand(const RandomRange<float>& r, float baseValue) {
			if (r.min == 0.0f && r.max == 0.0f) { return baseValue; }
			return this->Rand(r.min, r.max);
		}

		bool IsEnabled(const RandomRange<float>& r) { return !(r.min == 0.0f && r.max == 0.0f); }
		Vector3 SafeRandVec3(const RandomRange<Vector3>& r) {
			return {
				(r.min.x == r.max.x) ? r.min.x : this->Rand(r.min.x, r.max.x),
				(r.min.y == r.max.y) ? r.min.y : this->Rand(r.min.y, r.max.y),
				(r.min.z == r.max.z) ? r.min.z : this->Rand(r.min.z, r.max.z)
			};
		}
		void SetAttractTargetPosition(const Vector3& target) { attractTargetPosition_ = target; }
	};
}