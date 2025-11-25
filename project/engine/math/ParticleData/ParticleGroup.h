#pragma once
#include <MaterialDate.h>
#include<Particle.h>
#include<InstanceData.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <list>
#include <memory>

// パーティクルグループ
struct ParticleGroup {
	std::unique_ptr<ParticleModel> model;                  // パーティクルモデル
	MaterialDate materialData;                             // マテリアルデータ(テクスチャファイルパスとテクスチャ用SRVインデックス)
	std::list<Particle> particles;                         // パーティクルのリスト
	uint32_t srvindex;                                     // インスタンシング用SRVインデックス
	Microsoft::WRL::ComPtr <ID3D12Resource> Resource;      // インスタンシングリソース
	uint32_t kNumInstance;                                 // インスタンス数
	InstanceData* instanceData = nullptr;                  // インスタンシングデータを書き込むためのポインタ
};