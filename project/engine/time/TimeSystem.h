#pragma once
#include <chrono>

/// <summary>
/// エンジン全体で使用する時間管理クラス（静的専用）
///
/// ・アプリ起動時間の記録
/// ・ウィンドウ初期化完了時間の記録
/// ・起動からの経過時間取得
///
/// ※ static 経由で利用
/// </summary>
class TimeSystem {
public: // メンバ関数    
    /// <summary>
    /// 時間計測を開始する
    /// </summary>
    static void Initialize();
    /// <summary>
    /// ウィンドウ初期化完了時刻を記録する
    /// Framework::Initialize の最後で呼び出す
    /// </summary>
    static void MarkWindowInitialized();
    /// <summary>
    /// アプリ起動から現在までの経過時間を取得する
    /// </summary>
    /// <returns>経過時間（秒）</returns>
    static double GetElapsedSeconds();
    /// <summary>
    /// アプリ起動からウィンドウ初期化完了までの時間を取得する
    /// </summary>
    /// <returns>起動完了までの時間（秒）</returns>
    static double GetStartupSeconds();
private: // メンバ変数
    using Clock = std::chrono::steady_clock;                  /// 使用するクロック型
    using TimePoint = std::chrono::time_point<Clock>;         /// 時刻記録用型
    static TimePoint s_startTime;                             /// アプリ起動時刻
    static TimePoint s_windowInitializedTime;                 /// ウィンドウ初期化完了時刻
    static bool s_initialized;                                /// 初期化済みフラグ
    static bool s_windowInitialized;                          /// ウィンドウ初期化完了フラグ
};