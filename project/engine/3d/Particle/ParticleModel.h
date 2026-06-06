#pragma once
#include <Model.h>
#include <DirectXCommon.h>	
#include <PrimitiveGenerator.h>

namespace MyEngine {
	/// <summary>
	/// パーティクルモデル
	/// </summary>
	class ParticleModel	{
	public: // メンバ関数
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="birectxcommon"></param>
		/// <param name="filename"></param>
		void Initialize(DirectXCommon* birectxcommon, const std::string& filename);
		/// <summary>
		/// 描画処理
		/// </summary>
		void Draw();
	private: // 内部関数
		// .mtlファイルの読み取り
		static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
		// .objファイルの読み取り
		static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
		// 頂点データ作成
		void CreateVertexBuffer();
		// マテリアル
		void MaterialGenerate();
	private:
		// ポインタ
		DirectXCommon* dxCommon_;
		// Objファイルのデータ
		ModelData modelData;
		// バッファリソース
		Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;;
		// バッファリソース内のデータを指すポインタ
		VertexData* vertexData = nullptr;
		Material* materialData = nullptr;
		// バッファリソースの使い道を補足するバッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		uint32_t vertexCount; // 頂点数
	public: // アクセッサ
		size_t GetVertexCount() const { return modelData.vertices.size(); }
	};
}