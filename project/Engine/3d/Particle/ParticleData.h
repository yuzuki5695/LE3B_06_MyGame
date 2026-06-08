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

	// ゲーム全体の最大パーティクルグループ数の目安（必要に応じて増やしてOK）
    static const uint32_t MaxGroupCount = 64; 

    // ゲーム全体で1フレームにスポーンできる合計数の器
    static const uint32_t MaxSpawnRequestCount = 4000;

	struct Velocity {
		Vector3 translate;
		Vector3 rotate;
		Vector3 scale;
	};
	
	struct CameraData {
		Matrix4x4 view;
		Matrix4x4 projection;
		Matrix4x4 billboard;
	};

	struct ParticleInfo {
		uint32_t particleCount;
		float padding[3];
	};
	
	struct SpawnInfo {
		uint32_t spawnIndex;
		float padding[3];
	};

	struct ParticleForGPU {
		// トランスフォーム
		Vector3 translate;  float pad0;
		Vector3 rotate;		float pad1;
		Vector3 scale;		float pad2;
		// カラー
		Vector4 color;
		Vector3 velocityTranslate;  float pad3;
		Vector3 velocityRotate;		float pad4;
		Vector3 velocityScale;		float pad5;
		float lifetime;
		float currentTime;
		uint32_t useGravity;		float pad6;
		float startAlpha;		float pad7[3];
	};
		 
	// ★ GPU一括スポーン要求構造体（128バイト固定）
	struct SpawnRequestGPU {
		Vector3 translate;          float pad0;  // 16バイト
		Vector3 rotate;             float pad1;  // 16バイト
		Vector3 scale;              float pad2;  // 16バイト
		Vector4 color;                           // 16バイト
		Vector3 velocityTranslate;  float pad3;  // 16バイト
		Vector3 velocityRotate;     float pad4;  // 16バイト
		Vector3 velocityScale;      float pad5;  // 16バイト

		// 最後の16バイトブロック
		float lifetime;                          // 4バイト
		uint32_t useGravity;                     // 4バイト
		uint32_t targetIndex;                    // 4バイト ★これがズレていた
		float padding;                           // 4バイト
	};

	// ★ 定数バッファは1個あたり256バイトの器として確保する
	struct GroupSpawnCB {
		uint32_t startRequestIndex; // 4バイト
		uint32_t spawnCount;        // 4バイト
		float padding[62];          // ★ 4 * 62 = 248バイト (合計 4 + 4 + 248 = 256バイト)
	};

	struct SpawnParticleGPU {
		Vector3 translate;		float pad0;
		Vector3 rotate;		float pad1;
		Vector3 scale;		float pad2;
		Vector4 color;

		Vector3 velocityTranslate;  float pad3;
		Vector3 velocityRotate;		float pad4;
		Vector3 velocityScale;		float pad5;

		float lifetime;
		uint32_t useGravity;
		float pad6[2];
	};


	// パーティクルグループ
	struct ParticleGroup {
		std::unique_ptr<ParticleModel> model;                    // パーティクルモデル
		MaterialData materialData;                               // マテリアルデータ(テクスチャファイルパスとテクスチャ用SRVインデックス)	
		Microsoft::WRL::ComPtr <ID3D12Resource> Resource;        // インスタンシングリソース 
		Microsoft::WRL::ComPtr <ID3D12Resource> uploadResource;        // インスタンシングリソース 
		uint32_t srvindex;                                       // インスタンシング用SRVインデックス
		uint32_t uavIndex;                                       // インスタンシング用UAVインデックス			
		uint32_t lastAllocatedIndex = 0;                         // 
		D3D12_RESOURCE_STATES currentState;
		ParticleForGPU* particleData = nullptr;
	};

	 template<typename T>
	 struct RandomRange { T min{};    T max{};    bool enable = false; };

	 struct ParticleSpawnRandom {
		 RandomRange<Vector3> translate;
		 RandomRange<Vector3> rotate;
		 RandomRange<Vector3> scale;

		 RandomRange<Vector4> color;

		 RandomRange<Vector3> velocityTranslate;
		 RandomRange<Vector3> velocityRotate;
		 RandomRange<Vector3> velocityScale;

		 RandomRange<float> lifetime;

		 RandomRange<uint32_t> count;
	 };

	 struct ParticleSpawnData {
		Transform transform;
		Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
		Velocity velocity{};
		float lifetime = 1.0f;
		uint32_t count = 1;
		bool useGravity = false;
	};

	struct SpawnRequest {
		std::string groupName;
		ParticleSpawnData spawnData;
	};
}