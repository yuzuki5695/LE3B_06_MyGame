#pragma once
#include<BaseScene.h>
#include<Sprite.h>
#include<Object3d.h>
#include<ParticleEmitter.h>
#include<SoundPlayer.h>
#include <Skybox.h>

/// <summary>
/// ゲームクリアシーン
/// </summary>
class GameClearScene : public BaseScene
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

    void UpdateStep();
    // 各種ステップ
    void Step1_MovePlayerAndSwitchCamera();
    void Step2_WaitOrDoSomething();
    void Step3_MoveCameraOnInput();

    float EaseOutSine(float t) {
        return sinf((t * 3.14159265f) / 2.0f);
    }

    Vector3 EaseVector3(const Vector3& start, const Vector3& end, float t) {
        float e = EaseOutSine(t);
        return {
            start.x + (end.x - start.x) * e,
            start.y + (end.y - start.y) * e,
            start.z + (end.z - start.z) * e
        };
    }
    float EaseOutCubic(float t)
    {
        float oneMinus = 1.0f - t;
        return 1.0f - (oneMinus * oneMinus * oneMinus);
    }
    bool EaseMove(Vector3& current, const Vector3& start, const Vector3& end, float& t, float speed)
    {
        if (t >= 1.0f) return true;

        t += speed;
        if (t > 1.0f) t = 1.0f;

        current = EaseVector3(start, end, t);
        return (t >= 1.0f);
    }

    float EaseOutBack(float t)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;

        return 1.0f + c3 * powf(t - 1.0f, 3) + c1 * powf(t - 1.0f, 2);
    }

private: // メンバ変数
    std::unique_ptr <Sprite> ui1_ = nullptr;     // スプライト 
    std::unique_ptr <Skybox> Box_ = nullptr;     //　背景
    std::unique_ptr <Object3d> player_ = nullptr;  // オブジェクト
    Transform offset_;

    float easeT_ = 0.0f;  // 0.0 → 1.0 まで進めていく
    float easeSpeed_ = 0.01f; // イージング速度
    Vector3 startOffset_;
    Vector3 endOffset_;

    uint32_t step_ = 0;
    // Step1 カメラ
    float step1CamT_ = 0.0f;
    bool step1CamMoveStart_ = false;

    float step2CamT_ = 0.0f;   // カメラX移動用のt
    float step2Time_ = 0.0f;   // 上下揺れ用の時間
    // GameClearScene.h にフラグ追加
    bool step2FinishPlayerEase_ = false;
    // GameClearScene.h
    float step3Timer_ = 0.0f;           // Step3開始からの経過時間
    bool step3FadeTriggered_ = false;   // 1秒後の短いフェード処理フラグ
    // Step3制御
    bool step3BackJumpDone_ = false;   // 後ろに下がる演出が完了したか
    float startStep3PosX_ = 0.0f;     // Step3開始時のX



    std::unique_ptr <Sprite> ui2_ = nullptr;     // スプライト      
    std::unique_ptr <Sprite> ui3_ = nullptr;     // スプライト 
   
    // UI移動制御用の変数
    float ui2Timer_;
    float ui2Duration_; // 約1秒
    float ui3Timer_;
    float ui3Duration_;

    Vector2 ui2StartPos; // 上にずらした初期位置
    Vector2 ui2EndPos; // 最終位置（現在の値）
    Vector2 ui3StartPos; // 上にずらした初期位置
    Vector2 ui3EndPos; // 最終位置（現在の値）

};