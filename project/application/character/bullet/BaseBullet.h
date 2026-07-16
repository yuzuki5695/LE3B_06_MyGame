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
        /// <summary>
        /// 初期化処理
        /// </summary>
        /// <param name="transform">初期Transform</param>
        /// <param name="velocity">初速度</param>
        virtual void Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) { transform_ = transform; velocity_ = velocity; };
        /// <summary>
        /// 更新処理
        /// </summary>
        virtual void Update() = 0;
        /// <summary>
        /// 描画処理
        /// </summary>
        virtual void Draw() = 0;
    protected:// 内部関数    
        /// <summary>
        /// 弾の寿命を更新する
        /// </summary>
        /// <param name="deltaTime">経過時間</param>
        void UpdateLifeTime(float deltaTime) {
            lifetime_ += deltaTime;
            if (lifetime_ >= maxlifetime_) {
                active_ = false;
            }
        }
    protected: // メンバ変数      
        std::unique_ptr<MyEngine::Object3d> bullet_;  //  モデル
        bool active_ = true;                         // アクティブ状態
        MyEngine::Transform transform_;              // 現在の位置
        MyEngine::Vector3 velocity_;                 // 毎フレームの移動量（速度ベクトル）
        float lifetime_ = 0.0f;                      // 経過時間
        float maxlifetime_ = 3.0f;                   // 最大寿命
    public: // アクセッサ
        // getter
        MyEngine::Object3d* GetObject3d() const { return bullet_.get(); }
        MyEngine::Transform GetTransform() const { return transform_; }
        virtual Collider* GetCollider() const = 0;
        virtual MyEngine::Vector3 GetColliderSize() const = 0;
        // setter
        void SetInactive() { active_ = false; }
        void SetTranslate(const MyEngine::Vector3& translate) { bullet_->SetTranslate(translate); }
        virtual void SetColliderSize(const MyEngine::Vector3& size) = 0;
        /// アクティブ状態を取得
        bool IsActive() const { return active_; }
    };
}