#include "TimeSystem.h"

/// <summary>
/// 静的メンバ変数の実体定義
/// </summary>
TimeSystem::TimePoint TimeSystem::s_startTime;
TimeSystem::TimePoint TimeSystem::s_windowInitializedTime;
bool TimeSystem::s_initialized = false;
bool TimeSystem::s_windowInitialized = false;

void TimeSystem::Initialize() {
    if (s_initialized) return; // 二重初期化防止

    s_startTime = Clock::now();
    s_initialized = true;
}

void TimeSystem::MarkWindowInitialized() {
    if (!s_initialized) return;

    s_windowInitializedTime = Clock::now();
    s_windowInitialized = true;
}

double TimeSystem::GetElapsedSeconds() {
    if (!s_initialized) return 0.0;

    TimePoint now = Clock::now();
    std::chrono::duration<double> diff = now - s_startTime;
    return diff.count();
}

double TimeSystem::GetStartupSeconds() {
    if (!s_initialized || !s_windowInitialized) return 0.0;

    std::chrono::duration<double> diff = s_windowInitializedTime - s_startTime;

    return diff.count();
}