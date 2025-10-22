#pragma once
#include <vector>
#include <BaseCharacter.h>

// ヘッダーの重複を防ぐ前方宣言
class Player;

///====================================================
/// CharacterManager
/// キャラクター（プレイヤー・敵など）を一括管理するマネージャクラス。
///====================================================
class CharacterManager {
private:
	static std::unique_ptr<CharacterManager> instance;

	CharacterManager(CharacterManager&) = delete;
	CharacterManager& operator=(CharacterManager&) = delete;
public: // メンバ関数
	CharacterManager() = default;
	~CharacterManager() = default;
	// シングルトンインスタンスの取得
	static CharacterManager* GetInstance();
	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();	
	/// <summary>
	/// 更新更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画更新
	/// </summary>
	void Draw(); 
	/// <summary>
    /// キャラクターの追加登録 (現状プレイヤ―用)
    /// </summary> 
	void AddCharacter(std::unique_ptr<BaseCharacter> character);
private:// メンバ変数
	std::vector<std::unique_ptr<BaseCharacter>> characters_; 	// キャラクター登録
	Player* player_ = nullptr;								    // プレイヤーキャラクター
public: // アクセッサ（Getter / Setter）
	// getter

	/// <summary>
    /// プレイヤーキャラクターへのポインタを取得
    /// </summary>
	Player* GetPlayer() const {
		assert(player_ != nullptr && "Player has not been registered!");
		return player_;
	}
};
