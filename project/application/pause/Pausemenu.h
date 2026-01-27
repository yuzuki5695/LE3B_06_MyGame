#pragma once
#include<Object3d.h>
#include<Sprite.h>
#include <vector>
#include <memory>

// ポーズメニューの選択肢
enum class PauseCommand {
    Resume,     // ゲーム再開
    GoToTitle,  // タイトルへ
    None        // 選択中
};

/// <summary>
/// 各演出を管理するクラス
/// </summary>
class Pausemenu {
public: // メンバ関数 
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
    void Draw();
private: // メンバ変数
    std::unique_ptr <Sprite> ui_;
    // 選択肢の表示用（必要に応じて増やす）
    std::vector<Sprite*> cursor_;
    PauseCommand command_ = PauseCommand::None;
    int selectedIndex_ = 0; // 現在選んでいる項目

public: // メンバ関数 
    // シーン側から状況を確認するための関数
    PauseCommand GetCommand() const { return command_; }
};