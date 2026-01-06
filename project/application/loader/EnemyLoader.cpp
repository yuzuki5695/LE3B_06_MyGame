#include "EnemyLoader.h"
#include<json.hpp>
#include <fstream>
#include <ModelManager.h>

const std::string EnemyLoader::kDefaultBaseDirectory = "Resources/levels/";
const std::string EnemyLoader::kExtension = ".json";

LevelEnemyData* EnemyLoader::LoadFile(const std::string& fileName) {
	
	// 連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェックs
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;
	
	// 解凍
	file >> deserialized;

	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	// レベルデータ格納用インスタンスを生成
	levelData = new LevelEnemyData();
	
    // 各オブジェクトに対して再帰処理を呼び出す
    for (const auto& objectJson : deserialized["objects"]) {
        LoadObjectsRecursive(objectJson, *levelData);
    }

	return levelData;
}

void EnemyLoader::LoadObjectsRecursive(const nlohmann::json& objectJson, LevelEnemyData& levelData) {
    assert(objectJson.contains("type"));

    bool isDisabled = objectJson.contains("disabled") && objectJson["disabled"].get<bool>();

    if (isDisabled) return;

    // 種別を取得
    std::string type = objectJson["type"].get<std::string>();

    if (type == "MESH" && objectJson.contains("name")) {

        std::string name = objectJson["name"].get<std::string>();

        // ===== Enemy 判定 =====
        if (name.rfind("Enemy_", 0) == 0) {

            EnemyData enemy{};

            enemy.name = name;
            enemy.enemyIndex = ExtractNumber(name);

            // ===== type 判定 =====
            if (objectJson.contains("file_name")) {
                std::string fileName =
                    objectJson["file_name"].get<std::string>();
                enemy.enemyType = ExtractType(fileName);
            } else {
                enemy.enemyType = 0;
            }

            // ===== transform =====
            const auto& transform = objectJson["transform"];
            enemy.translation.x = (float)transform["translation"][0];
            enemy.translation.y = (float)transform["translation"][2];
            enemy.translation.z = (float)transform["translation"][1];

            levelData.enemies.push_back(enemy);
        }
    }

    // 子ノードがあれば再帰呼び出し
    if (objectJson.contains("children") && objectJson["children"].is_array()) {
        for (const auto& childJson : objectJson["children"]) {
            LoadObjectsRecursive(childJson, levelData);
        }
    }
}