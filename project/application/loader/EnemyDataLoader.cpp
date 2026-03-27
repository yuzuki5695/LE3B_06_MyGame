#include "EnemyDataLoader.h"
#include <cassert>

using json = nlohmann::json;

/// Enemy用JSONファイルのディレクトリとファイルの拡張子
const std::string EnemyDataLoader::kDefaultBaseDirectory = "Resources/Data/Enemy/";
const std::string EnemyDataLoader::kExtension = ".json";

EnemyData EnemyDataLoader::Load(const std::string& fileName) {

    // フルパス生成
    const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

    EnemyData data{};

    // JSONファイル読み込み（基底クラスの共通処理）
    json j = OpenFile(fullpath); 
    // 項目チェック
    assert(j.contains("name"));
    assert(j.contains("transform"));

    // 名前
    data.fileName = j["name"];
    // Transform
    data.transform = LoadTransform(j["transform"]);
    
    return data;
}