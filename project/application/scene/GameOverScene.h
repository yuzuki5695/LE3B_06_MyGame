#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include <Skybox.h>

struct PartInfo {
    std::unique_ptr<Object3d> obj;
    Transform transform;
    Vector3 fallSpeed;
    Vector3 rotateSpeed;
};

/// <summary>
/// ゲームオーバーシーン
/// </summary>
class GameOverScene : public BaseScene
{
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

    // イージング関数（EaseOutCubic）
    float EaseOutCubic(float t) {
        return 1.0f - powf(1.0f - t, 3.0f);
    }
    float EaseOutBack(float t) {
        float c1 = 1.70158f;
        float c3 = c1 + 1.0f;
        return 1.0f + c3 * powf(t - 1.0f, 3) + c1 * powf(t - 1.0f, 2);
    }

    void UpdateParts();/// パーツ落下・回転処理
    void UpdateUI();/// UIアニメーション制御

private: // メンバ変数      
    std::vector<PartInfo> partsList; // 複数のパーツ用
    uint32_t partCount_; // 生成するオブジェクト数
    std::unique_ptr <Sprite> ui1_ = nullptr;     // UIスプライト 
    std::unique_ptr <Sprite> ui2_ = nullptr;     // UIスプライト 
    std::unique_ptr <Skybox> Box_ = nullptr;     //　背景
    // UI移動制御用の変数
    float ui1Timer_;
    float ui1Duration_; // 約1秒
    float ui2Timer_;
    float ui2Duration_;

    Vector2 ui1StartPos; // 上にずらした初期位置
    Vector2 ui1EndPos; // 最終位置（現在の値）

    Vector2 ui2StartPos;
    Vector2 ui2EndPos;
    uint32_t phase_;
};