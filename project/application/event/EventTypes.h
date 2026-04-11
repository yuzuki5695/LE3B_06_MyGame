#pragma once

namespace MyGame {
    namespace Event {
        /// <summary>
        /// イベントの進行段階
        /// </summary>
        enum class EventPhase {
            Phase1,   // 第1段階（演出開始)
            Phase2,
            Phase3,
            Phase4,
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