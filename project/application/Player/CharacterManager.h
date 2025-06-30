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
    void Clear();

    // プレイヤーキャラクターを取得
    Player* GetPlayer();    
    // 敵だけのリストを返す
    std::vector<Enemy*> GetEnemies() const;

    void SetPlayer(std::unique_ptr<Player> player);
    void AddEnemy(std::unique_ptr<Enemy> enemy);

private:
    CharacterManager() = default;
    ~CharacterManager() = default;

    CharacterManager(const CharacterManager&) = delete;
    CharacterManager& operator=(const CharacterManager&) = delete;

    static CharacterManager* instance;
    std::unique_ptr<Player> player_;
    std::vector<std::unique_ptr<Enemy>> enemies_;
};

