#pragma once
#include <Vector4.h>
#include <Matrix4x4.h>
#include <Transform.h>
#include <ModelData.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <list>
#include <memory>

namespace MyEngine {
	struct Velocity {
		Vector3 translate;
		Vector3 rotate;
		Vector3 scale;
	};
	
	struct CameraData
	{
		Matrix4x4 view;
		Matrix4x4 projection;
		Matrix4x4 billboard;
	};

	struct ParticleInfo {
		uint32_t particleCount;
		float padding[3];
	};

	struct ParticleForGPU {
		Vector3 translate;
		float pad0;

		Vector3 rotate;
		float pad1;

		Vector3 scale;
		float pad2;

		Vector4 color;

		Vector3 velocityTranslate;
		float pad3;

		Vector3 velocityRotate;
		float pad4;

		Vector3 velocityScale;
		float pad5;

		float lifetime;
		float currentTime;
		uint32_t useGravity;
		float pad6;
	};

	// パーティクルグループ
	struct ParticleGroup {
		std::unique_ptr<ParticleModel> model;                  // パーティクルモデル
		MaterialData materialData;                             // マテリアルデータ(テクスチャファイルパスとテクスチャ用SRVインデックス)
		
		uint32_t srvindex;                                     // インスタンシング用SRVインデックス
		uint32_t uavIndex;                                     // インスタンシング用UAVインデックス
		
		Microsoft::WRL::ComPtr <ID3D12Resource> Resource;      // インスタンシングリソース 
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;	
		
		ParticleForGPU* particleData = nullptr;
		uint32_t lastAllocatedIndex = 0;
	};
}