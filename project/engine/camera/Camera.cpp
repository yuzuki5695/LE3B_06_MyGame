#include "Camera.h"
#include "MatrixVector.h"
#include "WinApp.h"
#include<ImGuiManager.h>

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
    // 右手座標系、Z軸が前方向の場合（DirectX標準ではカメラの前は -Z方向）
    // worldMatrix_ の第3列または第2列が前方向ベクトルを表すことが多い

    // 例えばworldMatrix_の3列目（Z軸方向）を取得し、単位化して返す
    Vector3 forward = { worldMatrix.m[0][2], worldMatrix.m[1][2], worldMatrix.m[2][2] };

    // DirectX標準ではカメラの前方向は -Zなので符号反転することが多い
    forward = -forward;

    // 正規化
    forward = Normalize(forward);

    return forward;
}
