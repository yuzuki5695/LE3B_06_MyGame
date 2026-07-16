#pragma once

namespace MyGame {
    namespace Event {
        /// <summary>
        /// イベントの進行段階
        /// </summary>
        enum class EventPhase {
            Phase1,   // 第1段階（演出開始)
            Phase2,   // 第2段階
            Phase3,   // 第3段階
            Phase4,   // 第4段階
            End,      // 完了
        };

        /// <summary>
        /// 発生するイベントの種類
        /// </summary>
        enum class EventState {
            None,       // イベントなし
            GameStart,  // ゲーム開始イベント
        };
    }
}