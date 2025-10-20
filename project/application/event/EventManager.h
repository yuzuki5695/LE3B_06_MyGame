#pragma once
#include<Object3d.h>
#include<Sprite.h>
#include <vector>

enum class EventState {
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
	void Initialize();
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
private:
    EventState state_ = EventState::None;
    float timer_ = 0.0f;
};