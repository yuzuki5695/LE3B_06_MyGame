#pragma once
#include <Vector3.h>

// カメラのタイプ
enum class ViewCameraType  {
    Main,      // メイン
    Sub,       // サブ
};
// カメラの状態
enum class CameraMode {
    Default,      // 手動 or 固定視点カメラ
    Follow,       // 追従カメラ（プレイヤーなどを追う）
    Event,        // イベント・演出用カメラ
    Transition,   // 切替中
    Debug,        // フリー移動
};
// 切り替え時の移動方法
enum class CameraSwitchType {
    Instant,   // 瞬間移動
    Ease       // イージング移動
};