#pragma once
#include <Vector3.h>

// カメラはの各情報
namespace CameraTypes {
    // カメラの種類
    enum class ViewCameraType {
        Main,      // メイン
        Sub,       // サブ
    };
    //  シーンごとのカメラのタイプ
    enum class SceneCameraType {
        Title,         // タイトル
        Gameplay,      // ゲームプレイ
        GameClear,     // ゲームクリア
        GameOver,      // ゲームオーバー
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
        Instant,   // 瞬間的に切り替わる
        Ease       // イージングでの移動
    };
}