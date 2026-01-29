#include "ResourceIdGenerator.h"
#include <fstream>

using json = nlohmann::json;

bool ResourceIdGenerator::LoadFromJson(const std::string& jsonPath) {
    std::ifstream ifs(jsonPath);
    if (!ifs) return false;

    json manifest;
    try {
        ifs >> manifest;
    } catch (const json::parse_error) {
        // パース失敗時は false を返す
        return false;
    }

    // JSON の構造を走査 (Group -> AssetName -> Data)
    for (auto& [groupName, assets] : manifest.items()) {
        for (auto& [assetName, assetData] : assets.items()) {

            ResourceEntry entry;
            entry.name = assetName;
            // model キーが存在すればパスを格納
            if (assetData.contains("model"))
                entry.model = assetData["model"].get<std::string>();
            // texture キーが存在すればパスを格納
            if (assetData.contains("texture"))
                entry.texture = assetData["texture"].get<std::string>();
            // audio キーが存在すればパスを格納
            if (assetData.contains("audio"))
                entry.audio = assetData["audio"].get<std::string>();

            resourceids_.push_back(entry);
        }
    }
    return true;
}

ResourceType ResourceIdGenerator::ToResourceType(const std::string& key) {
    // 文字列識別子から内部型の ResourceType へ変換を行う
    if (key == "texture") return ResourceType::Texture;
    if (key == "model")   return ResourceType::Model;
    if (key == "audio")   return ResourceType::Audio;

    // 想定外のキーが含まれている場合は例外を投げる
    throw std::runtime_error("Unknown resource type encountered: " + key);
}