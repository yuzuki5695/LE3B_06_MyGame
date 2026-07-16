#pragma once
#include <EventTypes.h>

namespace MyGame {
    /// <summary>
	/// イベントのインターフェースクラス
    /// </summary>
    class IEvent {
	public: // メンバ関数       
		/// デストラクタ        
        virtual ~IEvent() = default;
        /// <summary>
		/// 初期化処理
        /// </summary>
        virtual void Initialize() = 0;
        /// <summary>
		/// 更新処理
        /// </summary>
        virtual void Update() = 0;
        /// <summary>
		/// 2D描画処理
        /// </summary>
        virtual void Draw2D() = 0;
       
		/// イベントの終了判定        
        bool IsFinished() const { return isfinished_; }
	protected: 
        // イベントの終了フラグ		
        bool isfinished_ = false;
    };
}