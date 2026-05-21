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
        if (Input::GetInstance()->PushKey(DIK_SPACE) && timer_ <= 0.0f) {

            Camera* camera = CameraManager::GetInstance()->GetActiveCamera();

            if (!camera) {
                return;
            }

            //---------------------------------
            // カメラ回転
            //---------------------------------
            Vector3 camRot = camera->GetRotate();

            float camYaw = camRot.y;
            float camPitch = camRot.x;

            //---------------------------------
            // カメラ forward
            //---------------------------------
            Vector3 camForward = { sinf(camYaw) * cosf(camPitch),-sinf(camPitch),cosf(camYaw) * cosf(camPitch) };


            camForward = Normalize(camForward);

            //---------------------------------
            // プレイヤー→target方向
            //---------------------------------
            Vector3 playerPos = playerTransform.translate;

            Vector3 targetDir = Normalize(aimWorldPos - playerPos);

            //---------------------------------
            // yaw / pitch算出
            //---------------------------------
            float targetYaw = atan2f(targetDir.x, targetDir.z);
            float targetPitch = -asinf(targetDir.y);

            //---------------------------------
            // カメラとの差分角
            //---------------------------------
            float deltaYaw = targetYaw - camYaw;

            float deltaPitch = targetPitch - camPitch;

            //---------------------------------
            // 最終弾回転
            //---------------------------------
            float bulletYaw = camYaw + deltaYaw;
            float bulletPitch = camPitch + deltaPitch;

            //---------------------------------
            // 発射方向再構築
            //---------------------------------
            Vector3 bulletDir = { sinf(bulletYaw) * cosf(bulletPitch),-sinf(bulletPitch),cosf(bulletYaw) * cosf(bulletPitch) };

            bulletDir = Normalize(bulletDir);

            //---------------------------------
            // 弾速度
            //---------------------------------
            constexpr float speed = 15.0f;

            Vector3 velocity = bulletDir * speed;

            //---------------------------------
            // Transform
            //---------------------------------
            Transform bulletTransform;
            bulletTransform.scale = { 1,1,1 };
            bulletTransform.rotate = { bulletPitch,bulletYaw,0.0f };
            bulletTransform.translate = playerPos;

            //---------------------------------
            // 発射
            //---------------------------------
            BulletManager::GetInstance()->SpawnPlayerBullet(bulletTransform, velocity);
            // クールタイム
            timer_ = coolTime_;
        }
    }
}