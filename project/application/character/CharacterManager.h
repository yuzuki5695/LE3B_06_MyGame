#pragma once
#include <vector>
#include <BaseCharacter.h>

/// CharacterManager
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
	/// キャラクターを登録
	/// </summary>
	void AddCharacter(std::unique_ptr<BaseCharacter> character);
private:// メンバ変数
	std::vector<std::unique_ptr<BaseCharacter>> characters_; 	// キャラクター登録
public: // アクセッサ（Getter / Setter）
	/// キャラクター生成
	template<class T>
	T* CreateCharacter() {

		auto character = std::make_unique<T>();

		T* ptr = character.get();

		characters_.push_back(std::move(character));

		return ptr;
	}

	/// 単体取得
	template<class T>
	T* GetCharacter() {

		for (auto& character : characters_) {
			if (auto casted = dynamic_cast<T*>(character.get())) {
				return casted;
			}
		}

		return nullptr;
	}

	/// 複数取得
	template<class T>
	std::vector<T*> GetCharacters() {

		std::vector<T*> result;

		for (auto& character : characters_) {
			if (auto casted = dynamic_cast<T*>(character.get())) {
				result.push_back(casted);
			}
		}

		return result;
	}
};