#pragma once
#include<Object3d.h>
#include<Sprite.h>
#include <vector>
#include <memory>

enum class EventPhase {
    Phase1,        // 第1段階（演出開始)
    Phase2,        // 第2段階
    Phase3,        // 第3段階
    Phase4,        // 第4段階
    End,           // 完了 (演出終了)
};

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
	void Drawo3Dbject();
    void Draw2DSprite();
       
    //void Start(EventState newState);
    
    void DrawImGui();
    
    void Reset();

private:    // メンバ変数
    // イベントの種類
    EventState state_ = EventState::None;
    float timer_ = 0.0f;
    // イベントごとの演出フェーズ
    EventPhase phase_ = EventPhase::Phase1;

    std::unique_ptr <Object3d> object_ = nullptr;    
    std::unique_ptr <Sprite> sprite_ = nullptr;
    float alpha_;
    
    std::unique_ptr<Sprite> topSprite_ = nullptr;
    std::unique_ptr<Sprite> bottomSprite_ = nullptr;
    Vector2 topPos_;
    Vector2 bottomPos_; 
    Vector2 size_;
    float closeSpeed_ = 0.0f;
 
    std::unique_ptr<Sprite> mission_ = nullptr;
    Vector2 missionsize_;
    float missionalpha_;
    float missionTimer_ = 0.0f;   // 経過時間を記録

    bool timefige_;


    bool isActive_ = false;    // イベント実行中フラグ
    bool isFinished_ = false;  // イベント完了フラグ
    
public:
    bool IsActive() const { return isActive_; }
    bool IsFinished() const { return isFinished_; }
};