#include "PlayerDataLoader.h"
#include <fstream>
#include <cassert>

using namespace MyEngine;

namespace MyGame {

    using json = nlohmann::json;

    const std::string PlayerDataLoader::kDefaultBaseDirectory = "Resources/Data/Player/";
    const std::string PlayerDataLoader::kExtension = ".json";

    PlayerData PlayerDataLoader::Load(const std::string& fileName) {
        // 連結してフルパスを得る
        const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

        PlayerData data{};

        // ファイルを開く
        std::ifstream file(fullpath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON: " + fileName);
        }

        // JSONデータを読み込む
        json j;
        try {
            file >> j;
		}
        catch (const json::parse_error& e) {
            throw std::runtime_error(
                "JSON parse error in " + fullpath + "\n" + e.what()
            );
        }

        // 必須チェック
        ValidateRequiredKeys(j, { "name", "transform" }, fullpath);

        // データ取得
        data.name = j["name"].get<std::string>();
        data.transform = LoadTransform(j["transform"]);

        return data;
    }

    void PlayerDataLoader::ValidateRequiredKeys(const json& j, const std::initializer_list<std::string>& keys, const std::string& filePath) {
		// キーの存在をチェック
        for (const auto& key : keys) {
            if (!j.contains(key)) {
				// キーが存在しない場合は例外を投げる
                throw std::runtime_error("[JSON Error] Missing key '" + key + "' in file: " + filePath);
            }
        }
    }

    Transform PlayerDataLoader::LoadTransform(const json& transform) {
        Transform result{};

        // 安全チェック
        assert(transform.contains("translation"));
        assert(transform.contains("rotation"));
        assert(transform.contains("scaling"));
 
        // scale
        result.scale.x = (float)transform["scaling"][0];
        result.scale.y = (float)transform["scaling"][2];
        result.scale.z = (float)transform["scaling"][1];

        // rotation
        result.rotate.x = -(float)transform["rotation"][0];
        result.rotate.y = -(float)transform["rotation"][2];
        result.rotate.z = -(float)transform["rotation"][1];

        // translation
        result.translate.x = (float)transform["translation"][0];
        result.translate.y = (float)transform["translation"][1];
        result.translate.z = (float)transform["translation"][2];

        return result;
    }

}