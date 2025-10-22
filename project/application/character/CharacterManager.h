#pragma once
#include <vector>
#include <BaseCharacter.h>

class Player;

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
    
	void AddCharacter(std::unique_ptr<BaseCharacter> character);
private:// メンバ変数
	std::vector<std::unique_ptr<BaseCharacter>> characters_; 	// キャラクター登録
	Player* player_ = nullptr;								    // プレイヤーキャラクター
public: // アクセッサ（Getter / Setter）
	// getter
	Player* GetPlayer() const {
		assert(player_ != nullptr && "Player has not been registered!");
		return player_;
	}
};
