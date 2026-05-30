#pragma once
#include <Transform.h>
#include <Matrix4x4.h>

namespace MyEngine {
	/// <summary>
	/// カメラクラス
	/// </summary>
	class Camera {
	public: // メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Camera();
		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();
		/// <summary>
		/// ImGuiの描画処理
		/// </summary>
		void DrawImGui();
	private: // メンバ変数
		Transform transform;             // カメラのTransform
		Matrix4x4 worldMatrix;           // ワールド行列
		Matrix4x4 viewMatrix;            // ビュー行列
		Matrix4x4 projectionMatrix;      // プロジェクション行列
		Matrix4x4 ViewProjectionMatrix;  // ビュー・プロジェクション行列
		float fovY; // 水平方向視野野角
		float aspectRatio; // アスペクト比
		float nearClip; // ニアクリップ距離
		float farclip; // ファークリップ距離
	public:
		// getter
		const Matrix4x4& GetWorldMatrix() const { return worldMatrix; }
		const Matrix4x4& GetViewMatrix() const { return viewMatrix; }
		const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
		const Matrix4x4& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
		const Vector3& GetRotate() const { return transform.rotate; }
		Vector3& GetRotate() { return transform.rotate; }            // 書き換え用
		const Vector3& GetTranslate() const { return transform.translate; }
		Vector3& GetTranslate() { return transform.translate; }            // 書き換え用		
		Vector3 GetForward() const;
		Vector3 GetRight() const;
		Vector3 GetUp() const;
		// setter
		void SetRotate(const Vector3& rotate) { this->transform.rotate = rotate; }
		void SetTranslate(const Vector3& translate) { this->transform.translate = translate; }
		void SetFovY(const float& fovY) { this->fovY = fovY; }
		void SetAspectRatio(const float& aspectRatio) { this->aspectRatio = aspectRatio; }
		void SetNearClip(const float& nearClip) { this->nearClip = nearClip; }
		void Setfarclip(const float& farclip) { this->farclip = farclip; }
	};
}