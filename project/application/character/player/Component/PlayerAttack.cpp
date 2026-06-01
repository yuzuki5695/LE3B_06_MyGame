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

    void PlayerAttack::Update(const Transform& playerTransform, const Vector3& aimWorldPos, uint32_t level) {
        // クールタイム更新
        timer_ -= 1.0f / 60.0f;

        // 入力チェック
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
            // プレイヤー→target方向
            //---------------------------------
            Vector3 playerPos = playerTransform.translate;
            Vector3 targetDir = Normalize(aimWorldPos - playerPos);

            //---------------------------------
            // yaw / pitch
            //---------------------------------
            float targetYaw = atan2f(targetDir.x, targetDir.z);
            float targetPitch = -asinf(targetDir.y);

            //---------------------------------
            // 最終弾回転
            //---------------------------------
            float bulletYaw = targetYaw;
            float bulletPitch = targetPitch;

            //---------------------------------
            // 発射方向
            //---------------------------------
            Vector3 bulletDir = {
                sinf(bulletYaw) * cosf(bulletPitch),
                -sinf(bulletPitch),
                cosf(bulletYaw) * cosf(bulletPitch)
            };

            bulletDir = Normalize(bulletDir);

            //---------------------------------
            // プレイヤーの右方向
            //---------------------------------
            Vector3 right = { cosf(bulletYaw),0.0f,-sinf(bulletYaw) };

            right = Normalize(right);
            //---------------------------------
            // プレイヤーの上方
            //---------------------------------
            Vector3 up = { 0.0f,1.0f,0.0f };
            //---------------------------------
            // 弾速度
            //---------------------------------
            constexpr float speed = 15.0f;
            Vector3 velocity = bulletDir * speed;

            //---------------------------------
            // レベル別攻撃
            //---------------------------------

            // Lv1：1発
            if (level <= 1) {

                Transform bulletTransform;
                bulletTransform.scale = { 1,1,1 };
                bulletTransform.rotate = { bulletPitch,bulletYaw,0.0f };
                bulletTransform.translate = playerPos;

                BulletManager::GetInstance()->SpawnPlayerBullet(bulletTransform, velocity);
            }
            // Lv2以上：左右2発
            else if (level == 2) {

                constexpr float spreadOffset = 1.0f;
                Vector3 leftPos = playerPos - right * spreadOffset;
                Vector3 rightPos = playerPos + right * spreadOffset;

                // 左弾
                Transform leftBullet;
                leftBullet.scale = { 1,1,1 };
                leftBullet.rotate = { bulletPitch,bulletYaw,0.0f };
                leftBullet.translate = leftPos;

                BulletManager::GetInstance()->SpawnPlayerBullet(leftBullet, velocity);

                // 右弾
                Transform rightBullet;
                rightBullet.scale = { 1,1,1 };
                rightBullet.rotate = { bulletPitch,bulletYaw,0.0f };
                rightBullet.translate = rightPos;

                BulletManager::GetInstance()->SpawnPlayerBullet(rightBullet, velocity);
            } else if (level == 3) {
                constexpr float horizontalOffset = 1.0f;
                constexpr float verticalOffset = 0.8f;
                //---------------------------------
                // 発射位置
                //---------------------------------
                // 上
                Vector3 topPos = playerPos + up * verticalOffset;
                // 左下
                Vector3 leftBottomPos = playerPos - right * horizontalOffset - up * verticalOffset * 0.5f;
                // 右下
                Vector3 rightBottomPos = playerPos + right * horizontalOffset - up * verticalOffset * 0.5f;

                auto SpawnBullet = [&](const Vector3& pos)
                    {
                        Transform bullet;
                        bullet.scale = { 1,1,1 };
                        bullet.rotate = { bulletPitch,bulletYaw,0.0f };
                        bullet.translate = pos;

                        BulletManager::GetInstance()->SpawnPlayerBullet(bullet, velocity);
                    };
                SpawnBullet(topPos);
                SpawnBullet(leftBottomPos);
                SpawnBullet(rightBottomPos);
            }

            //---------------------------------
            // クールタイム
            //---------------------------------
            timer_ = coolTime_;
        } 
    }
}