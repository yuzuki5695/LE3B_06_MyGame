#pragma once
#include <Vector3.h>
#include <Vector4.h>
#include <PlayerData.h>

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
    void Initialize(const PlayerDeathData& data);

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
    bool IsFinished() const { return timer_ >= data_.duration; }
private:
    PlayerDeathData data_{};
    float timer_ = 0.0f;          // 経過時間タイマー
};