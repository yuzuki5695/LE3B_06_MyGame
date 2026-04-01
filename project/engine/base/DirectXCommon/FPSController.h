#pragma once
#include <chrono>

// ========================================
// FPS固定処理クラス
// ========================================
class FPSController {
public: // メンバ関数
	/// <summary>
	/// FPS固定初期化(現在時刻を基準にする）
	/// </summary>
	void Initialize();
	//  FPS固定更新
	void Update();	
private: // メンバ変数
    // 前フレームの記録時間
	std::chrono::steady_clock::time_point reference_;
    static const float kTargetFPS;                 // 目標FPS
    static const float kCheckFPS;                  // チェック用FPS
    static const int64_t kMicroSecondsPerSecond;   // 1秒 = 1000000μs

    static const std::chrono::microseconds kMinTime;       // 1フレーム時間
    static const std::chrono::microseconds kMinCheckTime;  // チェック用時間
};