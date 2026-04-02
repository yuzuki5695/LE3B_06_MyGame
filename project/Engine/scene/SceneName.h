#pragma once
#include <string>

namespace MyEngine {
    /// <summary>
    /// シーン識別用の定数群
    /// </summary>
    struct SceneName {
		static inline const std::string TITLE = "TITLE";             // タイトル
		static inline const std::string GAMEPLAY = "GAMEPLAY";       // ゲームプレイ 
		static inline const std::string GAMECLEAR = "GAMECLEAR";     // ゲームクリア
		static inline const std::string GAMEOVER = "GAMEOVER";       // ゲームオーバー
    };
}