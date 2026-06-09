#pragma once
#include <DirectXCommon.h>
#include <Camera.h>

namespace MyEngine {
	// 前方宣言
	class DsvManager;

	// パイプライン（RootSignature & PSO）をセットで管理する構造体
	struct Pipeline {
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	};

	// 3Dオブジェクト共通部
	class ParticleCommon {
	private:
		static std::unique_ptr<ParticleCommon> instance;

		ParticleCommon(ParticleCommon&) = delete;
		ParticleCommon& operator=(ParticleCommon&) = delete;
	public: // メンバ関数
		ParticleCommon() = default;
		~ParticleCommon() = default;

		// シングルトンインスタンスの取得
		static ParticleCommon* GetInstance();
		// 終了
		void Finalize();
		// 初期化
		void Initialize(DirectXCommon* dxCommon, DsvManager* dsvManager);
		// 共通描画設定
		void Commondrawing();
		void CommandUpdate();
		void CommandSpawn();
	private: // 内部関数
		// 各パイプラインの生成関数
		// ルートシグネチャの生成
		void RootSignatureGenerate();
		void UpdatePipelineGenerate();
		void SpawnPipelineGenerate();
		// グラフィックスパイプラインの生成
		void GraphicsPipelineGenerate();
		void UpdateRootSignatureGenerate();
		void SpawnRootSignatureGenerate();
		// RootSignature生成の共通処理
		void CreateRootSignature(ID3D12Device* device, const D3D12_ROOT_SIGNATURE_DESC& desc, Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature);
	private: // メンバ変数
		// ポインタ
		DirectXCommon* dxCommon_;
		DsvManager* dsvManager_;
		Camera* defaultCamera = nullptr;		
		// 各パイプラインのインスタンス
        Pipeline graphicsPipeline_;
		Pipeline updatePipeline_;
        Pipeline spawnPipeline_;
	public:
		// setter
		void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
		// getter
		DirectXCommon* GetDxCommon() const { return  dxCommon_; }
		Camera* GetDefaultCamera() const { return defaultCamera; }
	};
}
