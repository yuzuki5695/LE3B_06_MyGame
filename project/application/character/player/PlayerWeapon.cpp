#include "PlayerWeapon.h"
#include <Input.h>
#include <PlayerBullet.h>
#include <BulletManager.h>
#include <MatrixVector.h>
#include <GamePlayCamera.h>

using namespace MatrixVector;

void PlayerWeapon::Initialize() { 
	// 各変数の初期化
    bulletTimer_ = 0.0f;
    canShoot_ = true;
}

void PlayerWeapon::Update(const Vector3& playerWorldPos, const Vector3& targetWorldPos, GamePlayCamera* gameCam) {
    if (!gameCam) return;

    // --- 連射タイマー処理 ---
    bulletTimer_ += 1.0f / 60.0f;
    if (bulletTimer_ >= bulletInterval_) {
        canShoot_ = true;
        bulletTimer_ = 0.0f;
    }

    if (!canShoot_) return;

    // --- 発射入力チェック ---
    if (Input::GetInstance()->Pushkey(DIK_SPACE)) {
        // カメラの前方方向を取得
        Vector3 cameraForward = gameCam->GetForward();

        // 弾の初期位置（プレイヤーの少し前）
        // bulletStartPos = transform_.translate + gameCam->GetForward() * 2.0f;
        Vector3 bulletStartPos;
        bulletStartPos = playerWorldPos + cameraForward * 2.0f;

        // ターゲット（レティクル3D）方向へのベクトルを計算
        // Vector3 shootDir = Normalize(targetPos - bulletStartPos);
        Vector3 diff;
        diff = targetWorldPos - bulletStartPos;
        
        Vector3 shootDir = Normalize(diff);
        // 弾の生成と初期化
        std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>(); 
        // 10.0f 先を終点として計算
        Vector3 endPos;
        endPos = bulletStartPos + shootDir * 10.0f;
        // 元の初期化形式: Initialize(始点, 終点, 方向, スピード)
        bullet->Initialize(bulletStartPos, endPos, cameraForward, 5.0f);
        // マネージャーへ追加
        BulletManager::GetInstance()->AddPlayerBullet(std::move(bullet));
        // 発射フラグを折る
        canShoot_ = false;
    }
}