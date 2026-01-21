#pragma once
#include "Vector3.h"

/// <summary>
///  アニメーションや補間処理で利用するイージング関数群
///  t は 0.0f ～ 1.0f の正規化時間を想定
/// </summary>
namespace Easing {

    /// <summary> 
    ///  単一の float 値に対するイージング関数    
    ///  戻り値は 0.0f ～ 1.0f の範囲で補間率を返す
    /// </summary>

    // t : [0 → 1] が滑らかに加速 → 減速しながら変化する
    float EaseOutSine(float t);     // ゆっくり終わるサイン曲線
    float EaseOutCubic(float t);    // 非常に滑らかに終点で減速
    float EaseOutBack(float t);     // 終点を少し通り越して戻る（"Back" 効果）
    float EaseOutQuad(float t);

    float EaseInSine(float t);      // 始点がゆっくり
    float EaseInCubic(float t);     // 始点が非常にゆっくり
    float EaseInBack(float t);      // 始点で少し逆方向の反動

    float EaseInOutSine(float t);   // 全体が滑らかなサインカーブ
    float EaseInOutCubic(float t);  // 全体が強めに滑らか
    float EaseInOutBack(float t);   // 始点と終点両方に反動を持つ

    /// <summary> 
    //  Vector3 のイージング補間
    /// </summary>
    Vector3 EaseVector3(const Vector3& start, const Vector3& end, float t, float(*easeFunc)(float));
    /// <summary>
    /// 汎用線形補間（イージングなしの基本補間） 
    /// </summary>
    /// floatの線形補間
    inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
    // Vector3の線形補間
    Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
}