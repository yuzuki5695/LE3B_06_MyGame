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
	// 終了
	void Finalize();

	/// <summary>
	/// 初期化
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
	// キャラクター登録
	std::vector<std::unique_ptr<BaseCharacter>> characters_;
	Player* player_ = nullptr;
public: // メンバ関数
	// getter
	Player* GetPlayer() const {
		assert(player_ != nullptr && "Player has not been registered!");
		return player_;
	}
};
