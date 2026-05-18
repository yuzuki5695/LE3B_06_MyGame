#pragma once
#include <BaseScene.h>
#include <Object3d.h>

struct PartInfo {
    std::unique_ptr<MyEngine::Object3d> obj;
    MyEngine::Transform transform;
    MyEngine::Vector3 fallSpeed;
    MyEngine::Vector3 rotateSpeed;
};

namespace MyGame {
    /// <summary>
    /// ゲームオーバーシーン
    /// </summary>
    class GameOverScene : public MyEngine::BaseScene {
    public: // メンバ関数
        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize() override;
        /// <summary>
        /// 終了処理
        /// </summary>
        void Finalize() override;
        /// <summary>
        /// 毎フレーム更新処理
        /// </summary>
        void Update() override;
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw() override;
        /// <summary>
        /// パーツの位置と回転を更新する
        /// </summary>
        void UpdateParts();
    private: // メンバ変数      
        std::vector<PartInfo> partsList; // 複数のパーツ用
        uint32_t partCount_; // 生成するオブジェクト数
    };
}