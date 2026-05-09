#pragma once
#include <Vector3.h>
#include <memory>
#include <Object3d.h>
#include <Collider.h>

namespace MyGame {
    /// <summary>
    /// 弾の基底クラス（抽象クラス）
    /// プレイヤー弾・敵弾の共通インターフェースを定義する。
    /// 各派生クラスは Initialize / Update / Draw などを実装する。
    /// </summary>
    class BaseBullet {
    public:// メンバ関数 
        /// <summary>
        /// デストラクタ
        /// </summary> 
        virtual ~BaseBullet() = default;
        virtual void Finalize() {}
        /// <summary>
        /// 初期化処理
        /// </summary>
        /// <param name="pos"></param>
        /// <param name="vel"></param>
        virtual void Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) { transform_ = transform; velocity_ = velocity; };
        /// <summary>
        /// 更新処理
        /// </summary>
        virtual void Update() = 0;
        /// <summary>
        /// 描画処理
        /// </summary>
        virtual void Draw() = 0;

        // 衝突時に呼ばれる関数 (基本は何もしない、必要なら派生クラスでオーバーライド)
        virtual void OnCollision(Collider* other) {}

    protected:
        // 弾の寿命管理
        void UpdateLifeTime(float deltaTime) {
            lifeTime_ += deltaTime;
            if (lifeTime_ >= maxLifeTime_) {
                active_ = false;
            }
        }

    protected: // メンバ変数
        std::unique_ptr<MyEngine::Object3d> bullet;
        bool active_ = true;

        MyEngine::Transform transform_; // 現在の位置
        MyEngine::Vector3 velocity_; // 毎フレームの移動量（速度ベクトル）
        float lifeTime_ = 0.0f;      // 経過時間
        float maxLifeTime_ = 3.0f;   // 最大寿命（秒）
        // コライダー本体
        std::unique_ptr<Collider> collider_;
    public: // アクセッサ

        MyEngine::Object3d* GetObject3d() const { return bullet.get(); }
        MyEngine::Transform GetTransform() const { return transform_; }

        /// <summary>
        /// アクティブ状態を取得
        /// </summary>
        /// <returns>弾が有効なら true</returns>
        bool IsActive() const { return active_; }

        /// <summary>
        /// 弾を非アクティブ状態にする（削除対象にする）
        /// </summary>
        void SetInactive() { active_ = false; }


        void SetTranslate(const MyEngine::Vector3& translate) { bullet->SetTranslate(translate); }

        Collider* GetCollider() const { return collider_.get(); }

        // 衝突属性の設定を楽にするためのヘルパー
        void SetCollision(uint32_t attribute, uint32_t mask) {
            if (!collider_) collider_ = std::make_unique<Collider>();
            collider_->SetCollisionAttribute(attribute);
            collider_->SetCollisionMask(mask);
            // 衝突したら自分のOnCollisionを呼ぶように登録
            collider_->SetCallback([this](Collider* other) { this->OnCollision(other); });
        }
    };
}