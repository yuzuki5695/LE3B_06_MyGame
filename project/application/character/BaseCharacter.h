#pragma once
#include <memory>
#include <Object3d.h>

namespace MyGame {
    /// <summary>
    /// ゲームキャラクターの基底クラス
    /// 初期化・更新・描画のインターフェイスを定義
    /// </summary>
    class BaseCharacter {
    public:
        /// <summary>
        /// デストラクタ
        /// </summary> 
        virtual ~BaseCharacter() = default;
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
        /// キャラクターが生存しているか判定する
        /// </summary>
        /// <returns>true: 生存 / false: 死亡</returns>
        bool IsAlive() const { return isAlive_; } 
        /// <summary>
        /// キャラクターを死亡状態にする
        /// 主に削除フラグとして使用
        /// </summary>
        void Destroy() { isAlive_ = false; }
    protected:
        /// <summary>
        /// 3Dオブジェクト本体
        /// モデルやトランスフォーム情報を保持
        /// </summary>
        std::unique_ptr<MyEngine::Object3d> object_;
        /// <summary>
        /// 生存フラグ
        /// falseになると削除対象となる
        /// </summary>
        bool isAlive_ = true;
	public: // アクセッサ
        // getter
        MyEngine::Object3d* GetObject3d() const { return object_.get(); }
    };
}