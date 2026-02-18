#include "PlayerDeath.h"
#include <Object3d.h>
#include <cmath>

void PlayerDeath::Initialize() {
    timer_ = 0.0f;
}

void PlayerDeath::Update(Vector3& rotate, Vector3& offset, Object3d* object) {
    if (!object) return;

    // フレーム加算（60FPS想定）
    timer_ += 1.0f / 60.0f;

    // 落下挙動の計算
    offset.y -= kFallSpeedY;
    offset.z += kFallSpeedZ;

    // フラフラとした回転の更新
    // 単純な加算ではなく、sin/cos を使うことで「機体が揺れながら回る」不規則さを演出
    // X軸（ピッチ）とZ軸（ロール）を別々の周期で揺らす
    rotate.x += 0.004f + 0.002f * std::sin(timer_ * 0.5f);
    rotate.z += 0.003f + 0.0015f * std::cos(timer_ * 0.4f);

    // 被弾点滅処理 
    // サイン波を使って 0.0 ～ 1.0 の間を高速に往復する数値（s）を作る
    float s = (std::sin(timer_ * kBlinkSpeed) + 1.0f) * 0.5f;

    // 算出した s が特定の閾値（0.85）を超えた瞬間だけ、モデルの色を「赤」に変える
    if (s > 0.85f) {
        // 赤色（RGBA: 1, 0, 0, 1）
        object->SetMaterialColor({ 1.0f, 0.0f, 0.0f, 1.0f });
    }
    else {
        // 通常（白色：RGBA: 1, 1, 1, 1）
        object->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }
}