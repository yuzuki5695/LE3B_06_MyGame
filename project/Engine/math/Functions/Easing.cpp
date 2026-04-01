#include "Easing.h"
#include <cmath>

namespace Easing {

    // ============================================================
    //  単一 float のイージング関数
    //  t は 0.0f ～ 1.0f を想定
    // ============================================================

    // -------------------------
    //  EaseOut 系（終わりがゆっくり）
    // -------------------------

    float EaseOutSine(float t) {
        // サイン波の終わりを使って滑らかに減速
        return sinf((t * 3.14159265f) / 2.0f);
    }

    float EaseOutCubic(float t) {
        // t を逆向き (1 - t) にして立ち上がりスローを作る
        float k = 1.0f - t;
        return 1.0f - (k * k * k);
    }

    float EaseOutBack(float t) {
        // 終点で少しオーバーシュートする Back 効果カーブ
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;

        return 1.0f + c3 * powf(t - 1.0f, 3) + c1 * powf(t - 1.0f, 2);
    }

    float EaseOutQuad(float t) {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    // -------------------------
    //  EaseIn 系（最初がゆっくり）
    // -------------------------

    float EaseInSine(float t) {
        return 1.0f - cosf((t * 3.14159265f) / 2.0f);
    }

    float EaseInCubic(float t) {
        return t * t * t;
    }

    float EaseInBack(float t) {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;

        return c3 * t * t * t - c1 * t * t;
    }


    // -------------------------
    //  EaseInOut 系（中央が速い）
    // -------------------------

    float EaseInOutSine(float t) {
        return -(cosf(3.14159265f * t) - 1.0f) / 2.0f;
    }

    float EaseInOutCubic(float t) {
        return (t < 0.5f)
            ? 4.0f * t * t * t
            : 1.0f - powf(-2.0f * t + 2.0f, 3) / 2.0f;
    }

    float EaseInOutBack(float t) {
        const float c1 = 1.70158f;
        const float c2 = c1 * 1.525f;

        if (t < 0.5f) {
            return (powf(2.0f * t, 2) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f;
        } else {
            return (powf(2.0f * t - 2.0f, 2) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
        }
    }


    // ============================================================
    //  Vector3 のイージング補間
    // ============================================================

    Vector3 EaseVector3(const Vector3& start, const Vector3& end, float t, float(*easeFunc)(float)) {

        float e = easeFunc(t); // イージング関数で補間率を算出

        return {
            start.x + (end.x - start.x) * e,
            start.y + (end.y - start.y) * e,
            start.z + (end.z - start.z) * e
        };
    }


    // ============================================================
    //  Vector3 線形補間（イージングなし）
    // ============================================================

    Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
        return {
            start.x + (end.x - start.x) * t,
            start.y + (end.y - start.y) * t,
            start.z + (end.z - start.z) * t
        };
    }

}