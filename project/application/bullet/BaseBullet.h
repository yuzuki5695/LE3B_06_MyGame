#pragma once
#include "Vector3.h"
#include <memory>

// 前方宣言
class Object3d;

// 弾の基底クラス
class BaseBullet {
public:// メンバ関数
	// デストラクタ
	virtual ~BaseBullet() = default;
	// 初期化
	virtual void Initialize() = 0;
	virtual void Initialize(const Vector3& startPos, const Vector3& targetPos, float speed) {};
	// 終了
	virtual void Finalize() = 0;
	// 毎フレーム更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw() = 0;

	  // ===== 共通属性のためのアクセサ ===== //

    // アクティブ状態の取得・設定
    bool IsActive() const { return active_; }
	void SetInactive() { active_ = false; }

protected: 
    bool active_ = true;
};