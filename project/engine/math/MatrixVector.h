#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"

inline Vector4 operator*(const Matrix4x4& mat, const Vector4& vec) {
    Vector4 result;
    result.x = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2] * vec.z + mat.m[0][3] * vec.w;
    result.y = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2] * vec.z + mat.m[1][3] * vec.w;
    result.z = mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2] * vec.z + mat.m[2][3] * vec.w;
    result.w = mat.m[3][0] * vec.x + mat.m[3][1] * vec.y + mat.m[3][2] * vec.z + mat.m[3][3] * vec.w;
    return result;
}

namespace MatrixVector
{
	 
	// Dot product
	float Dot(const Vector3& a, const Vector3& b);

	// 単位行列の作成
	Matrix4x4 MakeIdentity4x4();

	//長さ（ノルム）
	float Length(const Vector3& v);

	//正規化
	Vector3  Normalize(const Vector3& v);

	// 逆行列
	Matrix4x4 Inverse(const Matrix4x4& matrix);

	//平行移動行列
	Matrix4x4 MakeTranslateMatrix(const Vector3& teanslate);

	// スケーリング行列
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	// 透視投影行列　
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farclip);

	// 平行投影行列
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float facDlip);

	// X軸回転行列
	Matrix4x4 MakeRotateXMatrix(float radian);

	// Y軸回転行列
	Matrix4x4 MakeRotateYMatrix(float radian);

	// Z軸回8;転行列
	Matrix4x4 MakeRotateZMatrix(float radian);

	// 合成
	Matrix4x4 Multiply(const Matrix4x4 m1, const Matrix4x4 m2);
	
	Vector4 MultiplyM4xV4(const Matrix4x4& m, const Vector4& v);

	Vector3 Multiply4x4x3(const Matrix4x4& mat, const Vector3& vec);

	// 3次元アフィン変換
	Matrix4x4 MakeAftineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	Matrix4x4 Transpose(const Matrix4x4& mat);

	// 逆行列を計算して転置する処理
	Matrix4x4 InverseTranspose(const Matrix4x4& mat);

	// クロス積
	Vector3 Cross(const Vector3& a, const Vector3& b);

	// ベクトルを回転させる関数を追加  
	Vector3 RotateVector(const Vector3& vec, const Vector3& rotation);

	// ワールド行列
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	Matrix4x4 MakeRotateMatrix(const Vector3& r);

};