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

            // プレイヤー位置（発射位置）
            Vector3 startPos = activecamera->GetTranslate();

            // レティクル位置（照準）
            Vector3 aimPos = aimWorldPos;
                 
            // 方向ベクトル
            Vector3 direction = Normalize(aimPos - startPos);

            // 速度設定
            float speed = 10.0f;
            Vector3 velocity = direction * speed;
        
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