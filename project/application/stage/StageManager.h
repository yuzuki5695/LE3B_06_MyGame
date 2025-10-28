#pragma once
#include <vector>
#include<BaseCharacter.h>
#include <CharacterLoader.h>

/// <summary>
/// ステージマネージャ
/// </summary>
class StageManager
{
public:
    void Finalize();
    void Initialize();
    void Update();
    void Draw();
    
    void DebugImGui();

private:
    // ポインタ
    CharacterLoader* loader_ = nullptr;
    // レベルデータ格納用インスタンスを生成
    LevelData* levelData_ = nullptr;
    // --- 本番オブジェクト ---
    std::vector<std::unique_ptr<Object3d>> object3ds_;  // 通常ゲーム用
    // --- デバッグ可視化オブジェクト ---
    std::vector<std::unique_ptr<Object3d>> debugObjects_; // 制御点・カーブ・カメラ位置など
    // --- デバッグ表示フラグ ---
    bool showDebugObjects_ = false; // ImGuiで切り替え可能にする

    // 地面
    std::unique_ptr <Object3d> grass = nullptr; 
};