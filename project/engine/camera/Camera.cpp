#include "Camera.h"
#include <MatrixVector.h>
#include <WinApp.h>
#include <ImGuiManager.h>

namespace MyEngine {

	using namespace MatrixVector;

	Camera::Camera()
		: transform({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
		, fovY(0.45f)
		, aspectRatio(float(WinApp::kClientWidth) / float(WinApp::kClientHeight))
		, nearClip(0.1f)
		, farclip(100.0f)
		, worldMatrix(MakeAftineMatrix(transform.scale, transform.rotate, transform.translate))
		, viewMatrix(Inverse(worldMatrix))
		, projectionMatrix(MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farclip))
		, ViewProjectionMatrix(Multiply(viewMatrix, projectionMatrix))
	{
	}

	void Camera::Update() {
		// ワールド行列の作成
		worldMatrix = MakeAftineMatrix(transform.scale, transform.rotate, transform.translate);
		// ビュー行列の計算
		viewMatrix = Inverse(worldMatrix);
		// プロジェクション行列の計算
		projectionMatrix = MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farclip);
		// ビュー・プロジェクション行列の計算
		ViewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
	}

	Vector3 Camera::GetForward() const {
		// +Z前方の場合
		return Normalize({ worldMatrix.m[2][0],worldMatrix.m[2][1],worldMatrix.m[2][2] });
	}

	Vector3 Camera::GetRight() const {
		return Normalize({ worldMatrix.m[0][0],worldMatrix.m[0][1],worldMatrix.m[0][2] });
	}

	Vector3 Camera::GetUp() const {
		return Normalize({ worldMatrix.m[1][0],worldMatrix.m[1][1],worldMatrix.m[1][2] });
	}

	void Camera::DrawImGui() {
#ifdef USE_IMGUI
		// 開発用UIの処理
		ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
		ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.0001f);
#endif // USE_IMGUI
	}
}