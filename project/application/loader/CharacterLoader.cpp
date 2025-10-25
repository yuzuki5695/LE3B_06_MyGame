#include "CharacterLoader.h"
#include<json.hpp>
#include <fstream>
#include <ModelManager.h>

const std::string CharacterLoader::kDefaultBaseDirectory = "Resources/levels/";
const std::string CharacterLoader::kExtension = ".json";

LevelData* CharacterLoader::LoadFile(const std::string& fileName) {
	
	// 連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
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
	levelData = new LevelData();
	
    // 各オブジェクトに対して再帰処理を呼び出す
    for (const auto& objectJson : deserialized["objects"]) {
        LoadObjectsRecursive(objectJson, *levelData);
    }

	return levelData;
}

void CharacterLoader::LoadObjectsRecursive(const nlohmann::json& objectJson, LevelData& levelData) {
    assert(objectJson.contains("type"));
    
    bool isDisabled = objectJson.contains("disabled") && objectJson["disabled"].get<bool>();

    if (!isDisabled) {
        // 種別を取得
        std::string type = objectJson["type"].get<std::string>();

        // 種類ごとの処理

        // MESH
        if (type == "MESH") {
            // 要素追加
            levelData.objects.emplace_back(LevelData::ObjectData{});
            // 今追加した要素の参照を得る
            LevelData::ObjectData& objectData = levelData.objects.back();

            if (objectJson.contains("file_name")) {
                // file_name を取得（例: "uvChecker"）
                objectData.fileName = objectJson["file_name"];

                //// .obj を付けたパスでロード（例: "uvChecker.obj"）
                //std::string modelPath = objectData.fileName + ".obj";
                //ModelManager::GetInstance()->LoadModel(modelPath);
            }


            // トランスフォームのパラメータ読み込み
            const auto& transform = objectJson["transform"];
            // 平行移動
            objectData.translation.x = (float)transform["translation"][0];
            objectData.translation.y = (float)transform["translation"][2];
            objectData.translation.z = (float)transform["translation"][1];
            // 回転角
            objectData.rotation.x = -(float)transform["rotation"][0];
            objectData.rotation.y = -(float)transform["rotation"][2];
            objectData.rotation.z = -(float)transform["rotation"][1];
            // スケーリング
            objectData.scaling.x = (float)transform["scaling"][0];
            objectData.scaling.y = (float)transform["scaling"][2];
            objectData.scaling.z = (float)transform["scaling"][1];
        } else if (type.compare("PlayerSpawn") == 0) {                        // 自キャラ発生ポイント
            // playersに要素を1つ追加する
            levelData.players.emplace_back(PlayerSpawnData{});
            PlayerSpawnData& playerData = levelData.players.back();
            // トランスフォームのパラメータ読み込み
            const auto& transform = objectJson["transform"];
            // 平行移動の数値を書き込む
            playerData.translation.x = (float)transform["translation"][0];
            playerData.translation.y = (float)transform["translation"][2]; // YとZ入れ替え
            playerData.translation.z = (float)transform["translation"][1];
            // 回転角の数値をラジアンで書き込む
            playerData.rotation.x = -(float)transform["rotation"][0];
            playerData.rotation.y = -(float)transform["rotation"][2];
            playerData.rotation.z = -(float)transform["rotation"][1];
        } else if (type.compare("EnemySpawn") == 0) {                        // 自キャラ発生ポイント
            // playersに要素を1つ追加する
            levelData.enemies.emplace_back(EnemySpawnData{});
            EnemySpawnData& enemyData = levelData.enemies.back();
            // トランスフォームのパラメータ読み込み
            const auto& transform = objectJson["transform"];
            // 平行移動の数値を書き込む
            enemyData.translation.x = (float)transform["translation"][0];
            enemyData.translation.y = (float)transform["translation"][2]; // YとZ入れ替え
            enemyData.translation.z = (float)transform["translation"][1];
            // 回転角の数値をラジアンで書き込む
            enemyData.rotation.x = -(float)transform["rotation"][0];
            enemyData.rotation.y = -(float)transform["rotation"][2];
            enemyData.rotation.z = -(float)transform["rotation"][1];
			// ファイル名を取得
			enemyData.fileName = objectJson["file_name"].get<std::string>();
        }
    }

    // 子ノードがあれば再帰呼び出し
    if (objectJson.contains("children") && objectJson["children"].is_array()) {
        for (const auto& childJson : objectJson["children"]) {
            LoadObjectsRecursive(childJson, levelData);
        }
    }
}