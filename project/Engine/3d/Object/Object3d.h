#pragma once
#include <Model.h>
#include <Camera.h>
#include <TransformationMatrix.h>

namespace MyEngine {

	// 前方宣言
	class Object3dCommon;

	//  3Dオブジェクト
	class Object3d {
	public: // メンバ関数
		struct CameraForGPU {
			Vector3 worldPosition;
		};
	public: // メンバ関数
		/// <summary>
		/// 3Dオブジェクトの初期化
		/// </summary>	
		/// <param name="object3dCommon">3Dオブジェクト共通管理クラスのポインタ</param>	
		/// <remarks>
		/// インスタンスごとの定数バッファ生成や、座標情報の初期化を行います。
		/// </remarks>
		void Initialize(Object3dCommon* object3dCommon);
		/// <summary>	
		/// 更新処理
		/// </summary>
		void Update();
		/// <summary>
		/// 描画処理
		/// </summary>
		void Draw();
		/// <summary>
		/// 3Dオブジェクトのインスタンスを生成・初期化する
		/// </summary>
		/// <param name="filePath">モデルファイルのパス（Resourcesフォルダからの相対パス）</param>
		/// <param name="transform">初期状態のトランスフォーム設定（座標、回転、スケール）</param>
		/// <returns>生成されたObject3dのユニークポインタ。失敗した場合はnullptrを返す。</returns>
		/// <remarks>
		/// モデルの読み込み、定数バッファの生成、およびトランスフォームの初期設定を内部で一括して行います。
		/// </remarks>
		static std::unique_ptr<Object3d> Create(std::string filePath, Transform transform);

		// imgui
		void DrawImGui(const std::string& name);
	private: // 内部関数
		// リソース
		// マテリアル
		void MaterialGenerate();
		// トランスフォームマトリックス
		void TransformationMatrixGenerate();
		// カメラリソース
		void CameraForGPUGenerate();
	private: // メンバ変数
		// ポインタ
		Object3dCommon* object3dCommon = nullptr;
		Model* model = nullptr;
		Camera* camera = nullptr;
		// バッファリソース
		Microsoft::WRL::ComPtr <ID3D12Resource> materialResource;
		Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
		// バッファリソース内のデータを指すポインタ		
		Material* materialData = nullptr;
		TransformationMatrix* transformationMatrixData = nullptr;
		CameraForGPU* cameraForGPUData = nullptr;
		/// ローカル座標系でのTransform
		Transform transform_;	
		/// ワールド座標座標
		Matrix4x4 worldMatrix_;
	public:
		// getter	
		Model* GetModel() const { return model; }
		const Transform& GetTransform() const { return transform_; }
		const Vector3& GetScale() const { return transform_.scale; }
		const Vector3& GetRotate() const { return transform_.rotate; }
		const Vector3& GetTranslate() const { return transform_.translate; }
		const Vector3& GetWorldPosition() const;

		// setter
		void SetModel(const std::string& filePath);
		void SetScale(const Vector3& scale) { this->transform_.scale = scale; }
		void SetRotate(const Vector3& rotate) { this->transform_.rotate = rotate; }
		void SetTranslate(const Vector3& translate) { this->transform_.translate = translate; }
		void SetCamera(Camera* camera) { this->camera = camera; }
		void SetMaterialColor(const Vector4& color);
	};
}