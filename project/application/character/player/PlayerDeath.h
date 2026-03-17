#pragma once
#include <Vector3.h>
#include <Vector4.h>

// 前方宣言
class Object3d;

/// <summary>
/// プレイヤーの死亡時演出管理クラス
/// 撃墜された際、制御不能になって落下しながら点滅する等の視覚効果を制御する。
/// </summary>
class PlayerDeath {
public:
    /// <summary>
    /// 初期化（タイマーのリセット）
    /// </summary>
    void Initialize();

    /// <summary>
    /// 死亡演出の更新
    /// </summary>
    /// <param name="rotate">機体の回転角（これに回転を加えてフラフラさせる）</param>
    /// <param name="offset">機体の位置オフセット（これに値を足して落下させる）</param>
    /// <param name="object">色を変えるための3Dモデルポインタ（点滅用）</param>
    void Update(Vector3& rotate, Vector3& offset, Object3d* object);

    /// <summary>
    /// 演出が終了したかどうか（シーン遷移のトリガーなどに使用）
    /// </summary>
    bool IsFinished() const { return timer_ >= kDuration; }
private:
    float timer_ = 0.0f;          // 経過時間タイマー
    const float kDuration = 2.5f; // 演出の合計時間（秒）

    // --- 演出用の設定定数 ---
    const float kFallSpeedY = 0.02f; // 下方向への落下速度
    const float kFallSpeedZ = 0.2f;  // 前方向への慣性移動速度
    const float kBlinkSpeed = 50.0f; // 赤点滅の速さ
};