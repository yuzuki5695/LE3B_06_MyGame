#pragma once
#include<Object3d.h>
#include<Sprite.h>
#include <vector>
#include <memory>

/// <summary>
/// イベントの進行段階
/// </summary>
enum class EventPhase {
    Phase1,        // 第1段階（演出開始)
    Phase2,        // 第2段階
    Phase3,        // 第3段階
    Phase4,        // 第4段階
    End,           // 完了 (演出終了)
};
/// <summary>
/// 発生するイベントの種類
/// </summary>
enum EventState {
    None,                // イベントなし
    GameStart,           // ゲーム開始イベント
};

/// <summary>
/// 各演出を管理するクラス
/// </summary>
class EventManager  {
private: // シングルトンインスタンス
    static EventManager* instance;
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
    /// 初期化処理
    /// </summary>
	void Initialize(const std::string& stateName);
    /// <summary>
    /// 更新処理
    /// </summary>
	void Update();
    /// <summary>
    /// 描画処理
    /// </summary>
    /// <summary>
    /// 3Dオブジェクトの描画
    /// </summary>
    void Drawo3Dbject();
    /// <summary>
    /// 2Dスプライトの描画
    /// </summary>
    void Draw2DSprite();
    /// <summary>
    /// ImGuiによるデバッグ描画
    /// </summary>
    void DrawImGui();

    /// <summary>
    /// イベント状態をリセット
    /// </summary>
    void Reset();
private:    // メンバ変数
    EventState state_ = EventState::None;      // イベントの種類
    float timer_ = 0.0f;                       // 経過時間
    EventPhase phase_ = EventPhase::Phase1;    // イベントごとの演出フェーズ
    // 2Dスプライト
    std::unique_ptr<Sprite> sprite_ = nullptr;
    float alpha_ = 0.0f;                        // 不透明度 
    // 上下から閉じる演出用スプライト  
    std::unique_ptr<Sprite> topSprite_ = nullptr;
    std::unique_ptr<Sprite> bottomSprite_ = nullptr;
    Vector2 topPos_;                          // 上スプライトの座標
    Vector2 bottomPos_;                       // 下スプライトの座標
    Vector2 size_;                            // スプライトのサイズ
    float closeSpeed_ = 0.0f;                 // スライド速度
    // 文字演出スプライト
    std::unique_ptr<Sprite> mission_ = nullptr;
    Vector2 missionsize_;                     // 文字スプライトサイズ
    float missionalpha_ = 0.0f;               // 透明度
    float missionTimer_ = 0.0f;               // 経過時間
    bool timefige_ = false;                   // 時間制御フラグ？
    bool isActive_ = false;                   // イベント実行中フラグ
    bool isFinished_ = false;                 // イベント完了フラグ
public:
    bool IsActive() const { return isActive_; }
    bool IsFinished() const { return isFinished_; }
};