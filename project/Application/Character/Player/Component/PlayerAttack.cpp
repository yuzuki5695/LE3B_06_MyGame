#include "PlayerAttack.h"
#include <Input.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include <CameraManager.h>

using namespace MyEngine;
using namespace MatrixVector;

namespace MyGame {
    ///====================================================
    /// 初期化処理
    ///====================================================
    void PlayerAttack::Initialize() {
        timer_ = 0.0f;
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void PlayerAttack::Update(const Transform& playerTransform, const Vector3& aimWorldPos, uint32_t level) {
        // クールタイム更新
        timer_ -= 1.0f / 60.0f;

        // 発射可能判定

        if (!CanShoot()) {
            return;
        }

        // 発射情報計算
        ShotData shotData = CalculateShotData(playerTransform, aimWorldPos);
        // レベル別攻撃
        FireByLevel(playerTransform.translate, shotData, level);

        // クールタイム
        timer_ = coolTime_;
    }

    bool PlayerAttack::CanShoot() const {
		// スペースキーが押されていて、クールタイムが0以下なら発射可能
        return Input::GetInstance()->PushKey(DIK_SPACE) && timer_ <= 0.0f;
    }

    void PlayerAttack::SpawnBullet(const Vector3& position, const ShotData& shotData) {
        Transform bullet_;
        bullet_.scale = { 1,1,1 };
        bullet_.rotate = { shotData.pitch,shotData.yaw,0.0f };
        bullet_.translate = position;
        BulletManager::GetInstance()->SpawnPlayerBullet(bullet_, shotData.velocity);
    }

    PlayerAttack::ShotData PlayerAttack::CalculateShotData(const Transform& playerTransform, const Vector3& aimWorldPos) {
        ShotData data;
        // プレイヤー→target方向
        Vector3 playerPos = playerTransform.translate;
        Vector3 targetDir = Normalize(aimWorldPos - playerPos);
        // yaw / pitch
        data.yaw = atan2f(targetDir.x, targetDir.z);
        data.pitch = -asinf(targetDir.y);
        // 発射方向
        data.direction = {
            sinf(data.yaw) * cosf(data.pitch),
            -sinf(data.pitch),
            cosf(data.yaw) * cosf(data.pitch)
        };

        data.direction = Normalize(data.direction);
        // 弾速度
        constexpr float speed = 15.0f;
        data.velocity = data.direction * speed;
        // プレイヤー右方向
        data.right = { cosf(data.yaw),0.0f,-sinf(data.yaw) };
        data.right = Normalize(data.right);
        return data;
    }

    void PlayerAttack::FireByLevel(const Vector3& playerPos, const ShotData& shotData, uint32_t level) {
        // Lv1
        if (level <= 1) {
            SpawnBullet(playerPos, shotData);
        }
        // Lv2
        else if (level == 2) {
            constexpr float offset = 2.0f;
            SpawnBullet(playerPos - shotData.right * offset, shotData);
            SpawnBullet(playerPos + shotData.right * offset, shotData);
        }
        // Lv3
        else if (level == 3) {
            constexpr float horizontal = 2.5f;
            constexpr float vertical = 1.5f;

            Vector3 up = { 0,1,0 };
            SpawnBullet(playerPos + up * vertical, shotData);
            SpawnBullet(playerPos - shotData.right * horizontal - up * vertical * 0.5f, shotData);
            SpawnBullet(playerPos + shotData.right * horizontal - up * vertical * 0.5f, shotData);
        }
    }
}