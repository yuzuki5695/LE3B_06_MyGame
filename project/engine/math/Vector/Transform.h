#pragma once
#include"Vector3.h"

///====================================================
/// <summary>
/// Transform構造体
/// 3Dオブジェクトの変換情報（スケール・回転・位置）をまとめて管理する構造体。
/// </summary>
///====================================================
struct Transform {
    Vector3 scale;     /// スケール（拡大率）
    Vector3 rotate;    /// 回転角度（ラジアンまたは度数法）
    Vector3 translate; /// 平行移動（ワールド座標上の位置）
};