#pragma once
#include <BaseCharacter.h>

/// <summary>
/// 敵の移動タイプを定義する列挙体
/// </summary>
enum class EnemyMoveType {
    None,       // 移動しない
    Vertical,   // 縦方向に移動
    Horizontal,  // 横方向に移動
    Diagonal
};

namespace MyGame {
	/// <summary>
	/// プレイヤーキャラクタークラス
	/// </summary>
	class Enemy : public BaseCharacter {
	public:// メンバ関数
		// デストラクタ
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

	};
}