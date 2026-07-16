#include "EnemyDeath.h"
#include <algorithm>

using namespace MyEngine;

namespace MyGame {
    ///==================================================== 
    /// 死亡演出を開始する
    ///====================================================
    void EnemyDeath::Start() {
        isPlaying_ = true;
        isfinished_ = false;
        timer_ = 0.0f;
        duration_ = 0.5f;
    }    
    ///====================================================
    /// 更新処理
    ///====================================================
    void EnemyDeath::Update(Object3d* object) {
        if (!isPlaying_ || !object) {
            return;
        }

        timer_ += 1.0f / 60.0f;

        float t = timer_ / duration_;

        // 演出終了時
        if (t >= 1.0f) {
            t = 1.0f;
            // 完全に0スケールで固定
            object->SetScale({ 0.0f, 0.0f, 0.0f });
            isfinished_ = true;
            isPlaying_ = false;
            return;
        }

        // 縮小
        float scale = 1.0f - t;

        object->SetScale({ scale, scale, scale });
        // ランダム回転
        std::uniform_real_distribution<float> rotDist(-5.0f, 5.0f);

        Vector3 rot = object->GetRotate();

        rot.x += rotDist(randomEngine_);
        rot.y += rotDist(randomEngine_);
        rot.z += rotDist(randomEngine_);

        object->SetRotate(rot);
        // 吹っ飛び
        std::uniform_real_distribution<float> moveDist(-0.2f, 0.2f);
        std::uniform_real_distribution<float> moveZDist(0.1f, 0.3f);

        Vector3 pos = object->GetTranslate();

        pos.x += moveDist(randomEngine_);
        pos.y += moveDist(randomEngine_);
        pos.z += moveZDist(randomEngine_);

        object->SetTranslate(pos);
    }
}