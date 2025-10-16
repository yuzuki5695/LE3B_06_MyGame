#pragma once
#include <Vector3.h>
#include <memory>

// ヘッダーの重複を防ぐ前方宣言
class Object3d;

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
    /// 初期化処理（共通）
    /// モデルや基本設定のロードなどを行う
    /// </summary>
	virtual void Initialize() = 0;
	virtual void Initialize(const Vector3& startPos, const Vector3& targetPos, float speed) {};
    /// <summary>
    /// 終了処理
    /// リソースの開放やオブジェクトの破棄を行う
    /// </summary>
	virtual void Finalize() = 0;
    /// <summary>
    /// 更新処理
    /// 弾の移動や寿命管理、衝突チェックなどを行う
    /// </summary>
	virtual void Update() = 0;
    /// <summary>
    /// 描画処理
    /// 弾のモデルを描画する
    /// </summary>
	virtual void Draw() = 0;

    // ===============================================
    // 共通属性のアクセッサ
    // ===============================================

    /// <summary>
    /// アクティブ状態を取得
    /// </summary>
    /// <returns>弾が有効なら true</returns>
    bool IsActive() const { return active_; }

    /// <summary>
    /// 弾を非アクティブ状態にする（削除対象にする）
    /// </summary>
    void SetInactive() { active_ = false; }

protected: 
    /// <summary>
    /// アクティブ状態フラグ
    /// true = 有効（描画・更新対象）
    /// false = 無効（削除）
    /// </summary>
    bool active_ = true;
};