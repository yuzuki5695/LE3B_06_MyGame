#pragma once
#include <memory>
#include <Object3d.h>
#include <CharacterState.h>
#include <CollisionManager.h>

namespace MyGame {
	// キャラクターの状態を管理するフラグ構造体    
    struct CharacterFlags {
        bool isAlive = true;                // 生存フラグ（falseなら削除対象）
        bool isActive = false;              // 活動フラグ（falseなら更新・描画処理をスキップ）
        bool isColliderRegistered = false;  // 当たり判定がCollisionManagerに登録されているか
        bool isEventLocked = true;          // イベントロックフラグ（falseならイベント処理を受けない）
    };

    /// <summary>
    /// ゲームキャラクターの基底クラス
    /// 初期化・更新・描画のインターフェイスを定義
    /// </summary>
    class BaseCharacter {
    public: // メンバ関数
        /// <summary>
        /// デストラクタ
        /// </summary> 
        virtual ~BaseCharacter() = default;
        /// <summary>
		/// 終了処理
        /// </summary>
        virtual void Finalize() = 0;
        /// <summary> 
        /// 初期化処理
        /// モデル生成や初期パラメータ設定を行う
        /// </summary>
        virtual void Initialize() = 0;
        /// <summary>
        /// 更新処理
        /// 毎フレーム呼ばれ、移動や状態更新を行う
        /// </summary>
        virtual void Update() = 0;
        /// <summary>
        /// 描画処理
        /// オブジェクトの描画を行う
        /// </summary>
        virtual void Draw() = 0;
        /// <summary>
        /// キャラクターの状態（ステート）を変更する。
        /// </summary>
        /// <param name="nextState">遷移先のステートポインタ（所有権を渡す）</param>
        void ChangeState(std::unique_ptr<ICharacterState> nextState) {
            state_.ChangeState(*this, std::move(nextState));            // 内部のステート管理クラスに処理を委譲
        }
    protected: // メンバ変数
        /// 3Dオブジェクト
        std::unique_ptr<MyEngine::Object3d> object_;
        /// 当たり判定
		std::unique_ptr<Collider> collider_;
        /// 状態フラグ
        CharacterFlags flags_;
        /// ステート
        CharacterState state_;
    public: // アクセッサ
        // getter
        MyEngine::Object3d* GetObject3d() const { return object_.get(); }
        Collider* GetCollider() const { return collider_.get(); }
        bool IsAlive() const { return flags_.isAlive; }
        bool IsActive() const { return flags_.isActive; }
        bool IsEventLocked() const { return flags_.isEventLocked; }
        // setter
        void SetAlive(bool flag) { flags_.isAlive = flag; }
        void SetActive(bool flag) { flags_.isActive = flag; }
        void SetEventLocked(bool flag) { flags_.isEventLocked = flag; }

        // 削除予約
        void Destroy() { flags_.isAlive = false; }
        // 当たり判定をCollisionManagerに登録
        void RegisterCollider() {
            if (!collider_) { return; }
            if (flags_.isColliderRegistered) { return; }            
            CollisionManager::GetInstance()->RegisterCollider(collider_.get());
            flags_.isColliderRegistered = true;
        }
    };
}