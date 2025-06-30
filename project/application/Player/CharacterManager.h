#pragma once
#include <memory>
#include <vector>
#include <Character.h>

class Player;
class Enemy;

class CharacterManager {
public:
    static CharacterManager* GetInstance();

    void Initialize();
    void Update();
    void Draw();

    void AddCharacter(std::unique_ptr<Character> character);
    void Clear();

    const std::vector<std::unique_ptr<Character>>& GetCharacters() const;

    // プレイヤーキャラクターを取得
    Player* GetPlayer();    
    // 敵だけのリストを返す
    std::vector<Enemy*> GetEnemies() const;

private:
    CharacterManager() = default;
    ~CharacterManager() = default;

    CharacterManager(const CharacterManager&) = delete;
    CharacterManager& operator=(const CharacterManager&) = delete;

    static CharacterManager* instance;
    std::vector<std::unique_ptr<Character>> characters_;
};

