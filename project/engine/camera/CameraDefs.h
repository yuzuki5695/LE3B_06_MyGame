#pragma once
#include <Object3d.h>

namespace MyEngine {
	// カメラはの各データ定義をまとめる名前空間
    namespace CameraDefs {
        // カメラの種類
        enum class CameraType {
            Main,       // メインカメラ
            Sub,        // サブカメラ
        };
        // カメラの状態
        enum class CameraState {
            Default,    // 手動操作
            Follow,     // 追従
			LockOn,     // 対象をロックオン
            Fixed,      // 固定
			Move,  // 移動
        };
        // 切り替え時の移動方法
        enum class CameraTransitionType {
            Instant,    // 即時切り替え
            Ease,       // イージング
            Lerp        // 線形補間
        };

        struct StateData {
            CameraType type = CameraType::Main;
            CameraState state = CameraState::Default;
            CameraTransitionType transition = CameraTransitionType::Instant;
        };
    }
}