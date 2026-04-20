#pragma once
#include <Vector3.h>

namespace MyGame::Math::Spline {
	/// <summary>
	/// // Catmull-Romスプラインの計算関数
	/// </summary>
	/// <param name="p0"></param>
	/// <param name="p1"></param>
	/// <param name="p2"></param>
	/// <param name="p3"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	inline MyEngine::Vector3 CatmullRom(const MyEngine::Vector3& p0, const MyEngine::Vector3& p1, const MyEngine::Vector3& p2, const MyEngine::Vector3& p3, float t);
	/// <summary>
	/// Catmull-Romスプラインの接線計算関数
	/// </summary>
	/// <param name="p0"></param>
	/// <param name="p1"></param>
	/// <param name="p2"></param>
	/// <param name="p3"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	inline MyEngine::Vector3 CatmullRomTangent(const MyEngine::Vector3& p0, const MyEngine::Vector3& p1, const MyEngine::Vector3& p2, const MyEngine::Vector3& p3, float t);
}