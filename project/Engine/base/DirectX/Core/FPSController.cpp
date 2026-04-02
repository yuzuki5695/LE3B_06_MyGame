#include "FPSController.h"
#include <thread>

namespace MyEngine {
    const float FPSController::kTargetFPS = 60.0f;                 // 60FPS固定
    const float FPSController::kCheckFPS = 65.0f;                  // チェック用FPS（これ以上速い場合はスリープする）
    const int64_t FPSController::kMicroSecondsPerSecond = 1000000; // 1秒あたりのマイクロ秒

    void FPSController::Initialize() {
        // 現在時刻を基準時間として記録
        reference_ = std::chrono::steady_clock::now();
    }

    void FPSController::Update() {
        // 現在時刻を取得
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

        // 前フレームからの経過時間を取得（マイクロ秒）
        std::chrono::microseconds elapsed =
            std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

        // まだ1フレーム分の時間が経過していない場合
        if (elapsed < kMinCheckTime) {

            // 1フレーム時間に達するまで待機
            while (std::chrono::steady_clock::now() - reference_ < kMinTime) {

                // CPU負荷軽減のため1μsスリープ
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        }

        // 次フレーム用に現在時刻を更新
        reference_ = std::chrono::steady_clock::now();
    }

    // 1フレームの最小時間（60FPS → 約16666μs）
    const std::chrono::microseconds FPSController::kMinTime(
        static_cast<int64_t>(kMicroSecondsPerSecond / kTargetFPS)
    );

    // スリープに入るかどうかの判定用時間（少し短め）
    const std::chrono::microseconds FPSController::kMinCheckTime(
        static_cast<int64_t>(kMicroSecondsPerSecond / kCheckFPS)
    );
}