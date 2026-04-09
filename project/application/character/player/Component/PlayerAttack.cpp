#include "PlayerAttack.h"
#include <Input.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include <CameraManager.h>

using namespace MyEngine;
using namespace MatrixVector;

namespace MyGame {

    void PlayerAttack::Initialize() {
        timer_ = 0.0f;
    }

    void PlayerAttack::Update(const Transform& playerTransform, const Vector3& aimWorldPos) {
        // =============================
        // クールタイム更新
        // =============================
        timer_ -= 1.0f / 60.0f;

        // =============================
        // 入力チェック
        // =============================
        if (Input::GetInstance()->Pushkey(DIK_SPACE) && timer_ <= 0.0f) {
            Camera* activecamera = CameraManager::GetInstance()->GetActiveCamera();
            Vector3 camPos = activecamera->GetTranslate();
            Vector3 rayDir = Normalize(aimWorldPos - camPos);
            Vector3 playerPos = playerTransform.translate;

            // レイに投影
            float t = Dot(playerPos - camPos, rayDir);
            Vector3 startPos = camPos + rayDir * t;

            float speed = 15.0f;
            Vector3 velocity = rayDir * speed;

            Transform bulletTransform;
            bulletTransform.scale = { 1.0f,1.0f,1.0f };
            bulletTransform.rotate = { 0.0f,0.0f,0.0f };
            bulletTransform.translate = playerTransform.translate;

            // =============================
            // ③ 弾生成
            // =============================
            BulletManager::GetInstance()->SpawnPlayerBullet(bulletTransform, velocity);

            // =============================
            // ④ クールタイムリセット
            // =============================
            timer_ = coolTime_;
        }
    }
}