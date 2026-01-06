#pragma once
#include<string>
#include <vector>
#include <Vector3.h>
#include<json.hpp>

struct EnemyData {
	std::string name;     // Enemy_00
	int enemyIndex;       // 0,1,2...
	int enemyType;        // 0,1,2...
	Vector3 translation;
};

// レベルデータ
struct LevelEnemyData {
	std::vector<EnemyData> enemies;      // ★ Enemy専用
};

class EnemyLoader
{
public:// メンバ関数			
    static const std::string kDefaultBaseDirectory;
    static const std::string kExtension;

	// ファイルの読み込み
	LevelEnemyData* LoadFile(const std::string& fileName);
	// モデルの情報を出力	
	void LoadObjectsRecursive(const nlohmann::json& objectsJson, LevelEnemyData& levelData);

private: // メンバ変数
	LevelEnemyData* levelData = nullptr;

public:
	int ExtractNumber(const std::string& str) {
		auto pos = str.find_last_of('_');
		if (pos == std::string::npos) return 0;
		return std::stoi(str.substr(pos + 1)); // Enemy_00 → 0 
	}
	int ExtractType(const std::string& fileName) {
		if (fileName.rfind("type_", 0) != 0) {
			return 0;
		}
		return std::stoi(fileName.substr(5));// type_01 → 1
	}
};