#pragma once
#include <StartEvent.h>

namespace MyGame {
    /// <summary>
	/// イベント管理クラス
    /// </summary>
    class EventManager {
    private: // シングルトンインスタンス
        static EventManager* instance_;
        // コピーコンストラクタを防ぐ
        EventManager(EventManager&) = delete;
        EventManager& operator=(EventManager&) = delete;
    public: // メンバ関数 
        EventManager() = default;
        ~EventManager() = default;
        // シングルトンインスタンスの取得
        static EventManager* GetInstance();
        /// <summary>
        /// 終了処理
        /// </summary>
        void Finalize();
        /// <summary>
		/// 更新処理
        /// </summary>
        void Update();
        /// <summary>
		/// 描画処理(2D)
        /// </summary>
        void Draw2D();
        /// <summary>
		/// イベント開始
        /// </summary>
        /// <param name="state"></param>
        void EventStart(Event::EventState state);
        /// <summary>
		/// イベントがアクティブかどうかを返す
        /// </summary>
        /// <returns></returns>
        bool IsActive() const;
    private:
		std::unique_ptr<IEvent> currentEvent_; // 現在のイベント
    };
}