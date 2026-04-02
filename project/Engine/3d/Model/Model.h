#pragma once
#include <ModelCommon.h>
#include <Transform.h>
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <assimp/mesh.h>
#include <ModelData.h>

namespace MyEngine {
	/// <summary>
	/// ノード構造（主にglTF用）
	/// 親子関係を持つトランスフォーム階層を表す
	/// </summary>
	struct Node {
		Matrix4x4 localMatrix;          // ローカル変換行列
		std::string name;               // ノード名
		std::vector<Node> children;     // 子ノード
	};
	/// <summary>
	/// glTF専用のモデルデータ
	/// ノード階層（スケルトン的構造）を含む
	/// </summary>
	struct glTFModelData {
		std::vector<VertexData> vertices; // 頂点データ
		MaterialData material;            // マテリアル情報
		Node rootNode;                    // ルートノード（階層の起点）
	};

	/// <summary>
	/// 3Dモデルの形状データを管理するクラス
	/// .objや.gltfファイルの読み込み、頂点バッファの保持を行う。
	/// </summary>
	class Model {
	public: // メンバ関数
		/// <summary>
		/// モデルの初期化
		/// ファイル拡張子を判別して適切なローダーを呼び出す。
		/// </summary>
		/// <param name="modelCommon">モデル共通設定クラス</param>
		/// <param name="directory path">ファイルが格納されているディレクトリ</param>
		/// <param name="filename">ファイル名（拡張子含む）</param>
		void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename);

		/// <summary>
		/// モデルの描画処理
		/// </summary>
		void Draw();

		/// <summary>
		/// AssimpのaiMeshをエンジン用頂点配列に変換する共通処理
		/// LoadObjFile / LoadModelFile の両方から使用される
		/// </summary>
		/// <param name="mesh">Assimpのメッシュ</param>
		/// <param name="vertices">出力先頂点配列</param>
		/// <param name="flipX">左手系変換（X反転）を行うか</param>
		static void ProcessMesh(aiMesh* mesh, std::vector<VertexData>& vertices, bool flipX);

		/// <summary>
		/// .mtl（マテリアルテンプレート）ファイルの読み取り
		/// </summary>
		/// <param name="directoryPath">ディレクトリパス</param>
		/// <param name="filename">ファイル名</param>
		/// <returns>読み込んだマテリアルデータ</returns>
		static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

		/// <summary>
		/// .objファイルの読み取り
		/// </summary>
		/// <param name="directoryPath">ディレクトリパス</param>
		/// <param name="filename">ファイル名</param>
		/// <returns>読み込んだモデルデータ</returns>
		static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

		/// <summary>
		/// Assimpのノード構造を再帰的に読み取る
		/// </summary>
		/// <param name="node">Assimpのノードポインタ</param>
		/// <returns>変換されたエンジン用ノードデータ</returns>
		Node ReadNode(aiNode* node);

		/// <summary>
		/// glTF/glbファイルの読み取り		
		/// </summary>
		/// <param name="directoryPath">ディレクトリパス</param>	
		/// <param name="filename">ファイル名</param>
		/// <returns>読み込んだglTFモデルデータ</returns>
		glTFModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	private: // 内部関数
		/// <summary>
		/// 頂点リソースの作成とデータの転送
		/// </summary>
		void VertexDatacreation();
	private:
		// ポインタ
		ModelCommon* modelCommon = nullptr;
		// Objファイルのデータ
		ModelData modelData;
		// バッファリソース
		Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource;
		// バッファリソースの使い道を補足するバッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		// バッファリソース内のデータを指すポインタ
		VertexData* vertexData = nullptr;
	};
}