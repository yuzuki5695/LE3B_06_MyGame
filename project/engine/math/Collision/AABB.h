#pragma once
#include <Vector3.h>

namespace MyEngine {	      
	/// <summary>
    /// AABB 構造体
    /// </summary>
    struct  AABB {
		Vector3 center_{};    // ボックスの中心座標（ワールド）
		Vector3 halfSize_{};  // ボックスの半分のサイズ（各軸方向）
	};
}