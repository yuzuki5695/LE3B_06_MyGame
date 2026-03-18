#pragma once
#include <BaseCharacter.h>

/// <summary>
/// 敵キャラクタークラス
/// </summary>
class Enemy : public BaseCharacter {
public: // メンバ関数

    /// <summary>
    /// デストラクタ
    /// </summary>
	~Enemy() override;
    /// <summary>
    /// 初期化処理
    /// </summary>
	void Initialize() override;	    
	/// <summary>
    /// 更新処理
    /// </summary>
	void Update() override;	    
	/// <summary>
    /// 描画処理
    /// </summary>
	void Draw() override;	

private: // メンバ変数
    std::unique_ptr<Object3d> object;// 敵モデルデータ
    Transform transform_;

};