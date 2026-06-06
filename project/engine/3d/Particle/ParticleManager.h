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
		void Emit(const std::string& name, const Transform& transform, const Vector4& color, uint32_t count, const Velocity& velocity, float lifetime);

		void ClearAll();

		void CameraForGPUGenerate();
		void SetAutoEmit(const std::string& name, const Transform& transform, const Vector4& color, uint32_t count, const Velocity& velocity, float frequency, float lifetime) {
			autoEmitGroup_ = name;
			emitTransform_ = transform;
			emitColor_ = color;
			emitCount_ = count;
			emitVelocity_ = velocity;
			emitFrequency_ = frequency;
			emitParticleLifetime_ = lifetime;
			isAutoEmitActive_ = true;
			emitTimer_ = 0.0f;
		}

		void StopAutoEmit() { isAutoEmitActive_ = false; }
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
		CameraData* cameraData = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> particleInfoResource_;
		ParticleInfo* particleInfoData_ = nullptr;


		// 💡 追加：自動発生用の管理変数群
		bool isAutoEmitActive_ = false;
		std::string autoEmitGroup_;
		Transform emitTransform_;
		Vector4 emitColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		uint32_t emitCount_ = 0;
		Velocity emitVelocity_;
		float emitFrequency_ = 0.0f;       // 発生間隔（秒）
		float emitTimer_ = 0.0f;           // 現在の経過タイマー
		float emitParticleLifetime_ = 0.0f;// 生成するパーティクル自体の寿命
	public: // アクセッサ
		// getter
		ParticleGroup& GetGroup(const std::string& name) {
			assert(particleGroups.count(name));
			return particleGroups[name];
		}
		uint32_t GetMaxInstanceCount() const { return MaxInstanceCount; }

	};
}