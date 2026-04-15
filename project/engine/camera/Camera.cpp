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

	void Camera::DebugUpdate() {
#ifdef USE_IMGUI
		// 開発用UIの処理
		ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
		ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.0001f);
#endif // USE_IMGUI
	}

	Vector3 Camera::GetForward() const {
		Vector3 forward = { worldMatrix.m[0][2], worldMatrix.m[1][2], worldMatrix.m[2][2] };
		// 正規化
		forward = Normalize(forward);

		return forward;
	}

	void Camera::SetLookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
		// forward
		Vector3 forward = Normalize(target - eye);

		// right
		Vector3 right = Normalize(Cross(up, forward));

		// up（再計算）
		Vector3 newUp = Cross(forward, right);

		// ワールド行列を直接作る（回転＋位置）
		worldMatrix.m[0][0] = right.x;
		worldMatrix.m[1][0] = right.y;
		worldMatrix.m[2][0] = right.z;

		worldMatrix.m[0][1] = newUp.x;
		worldMatrix.m[1][1] = newUp.y;
		worldMatrix.m[2][1] = newUp.z;

		worldMatrix.m[0][2] = forward.x;
		worldMatrix.m[1][2] = forward.y;
		worldMatrix.m[2][2] = forward.z;

		worldMatrix.m[3][0] = eye.x;
		worldMatrix.m[3][1] = eye.y;
		worldMatrix.m[3][2] = eye.z;

		// スケール固定
		worldMatrix.m[0][3] = 0.0f;
		worldMatrix.m[1][3] = 0.0f;
		worldMatrix.m[2][3] = 0.0f;
		worldMatrix.m[3][3] = 1.0f;

		// view更新
		viewMatrix = Inverse(worldMatrix);

		// VP更新
		ViewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
	}
}