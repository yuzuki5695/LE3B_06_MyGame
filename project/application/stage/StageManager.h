#pragma once
#include <BaseScene.h>
#include <CharacterLoader.h>
#include <Skybox.h>
#include <Object3d.h>

namespace MyGame {
    /// <summary>
    /// ステージマネージャ
    /// </summary>
    class StageManager {
    private:
        static std::unique_ptr<StageManager> instance;

        StageManager(StageManager&) = delete;
        StageManager& operator=(StageManager&) = delete;
    public: // メンバ関数
        StageManager() = default;
        ~StageManager() = default;
        // シングルトンインスタンスの取得
        static StageManager* GetInstance();
        // 終了
        void Finalize();
        void Initialize();
        void Update();
        void Draw();
        void DDSDraw();
	private: // メンバ変数
        //// ポインタ
        //CharacterLoader* loader_ = nullptr;
        //// レベルデータ格納用インスタンスを生成
        //LevelData* levelData_ = nullptr;
        //// --- 本番オブジェクト ---
        //std::vector<std::unique_ptr<MyEngine::Object3d>> object3ds_;  // 通常ゲーム用
        //// --- デバッグ可視化オブジェクト ---
        //std::vector<std::unique_ptr<MyEngine::Object3d>> debugObjects_; // 制御点・カーブ・カメラ位置など
        //// --- デバッグ表示フラグ ---
        //bool showDebugObjects_ = false; // ImGuiで切り替え可能にする

        // 地面
        std::unique_ptr <MyEngine::Object3d> grass = nullptr;

        std::unique_ptr<MyEngine::Skybox> skybox_;


    };
}