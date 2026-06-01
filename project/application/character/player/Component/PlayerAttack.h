#pragma once
#include <Object3d.h>

namespace MyGame {
    /// <summary>
    /// プレイヤーの攻撃クラス
    /// </summary>
    class PlayerAttack {
	private:
		// 発射に必要な情報をまとめた構造体
        struct ShotData { 
			float yaw;                       // 水平方向の角度（ラジアン）
			float pitch;                     // 垂直方向の角度（ラジアン）
			MyEngine::Vector3 direction{};   // 発射方向の単位ベクトル
			MyEngine::Vector3 velocity{};    // 発射速度ベクトル
			MyEngine::Vector3 right{};       // プレイヤーの右方向ベクトル
        };
	public: // メンバ関数
        /// <summary>
		/// 初期化処理
        /// </summary>
        void Initialize();
        /// <summary>
        /// 更新処理
        /// </summary>
        /// <param name="playerTransform">プレイヤーのTransform情報</param>
        /// <param name="aimWorldPos">狙いのワールド座標</param>
        /// <param name="level">攻撃レベル</param>
        void Update(const MyEngine::Transform& playerTransform, const MyEngine::Vector3& aimWorldPos, uint32_t level);
	private: // 内部関数
		/// 発射可能かどうかの判定
        bool CanShoot() const;
        /// <summary>
		/// プレイヤーのTransformと狙いのワールド座標から、弾の発射に必要な情報を計算する
        /// </summary>
        /// <param name="playerTransform"></param>
        /// <param name="aimWorldPos"></param>
        /// <returns></returns>
        ShotData CalculateShotData(const MyEngine::Transform& playerTransform, const MyEngine::Vector3& aimWorldPos);
        /// <summary>
		/// 弾の生成と発射処理
        /// </summary>
        /// <param name="position"></param>
        /// <param name="shotData"></param>
        void SpawnBullet(const MyEngine::Vector3& position, const ShotData& shotData);
        /// <summary>
		/// 攻撃レベルに応じた弾の発射処理
        /// </summary>
        /// <param name="playerPos"></param>
        /// <param name="shotData"></param>
        /// <param name="level"></param>
        void FireByLevel(const MyEngine::Vector3& playerPos, const ShotData& shotData, uint32_t level);
    private:
        float coolTime_ = 0.2f;   // 発射間隔
		float timer_ = 0.0f;      // クールタイム管理用タイマー
    };
}