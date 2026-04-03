#include "AssetInfoLoader.h"
#include <fstream>
#include <json.hpp>
#include <iostream>
#include <set>

using namespace AssetGen;

namespace fs = std::filesystem;

bool AssetInfoLoader::Load(const fs::path& jsonPath) {
    Clear();

    std::ifstream ifs(jsonPath);
    if (!ifs) {
        return false;
    }

    try {
        nlohmann::json j;
        ifs >> j;
        for (const nlohmann::json& item : j["assets"]) {
            AssetDefinition asset;
            asset.category = item.value("category", "General");
            asset.id       = item.value("id", "");
            asset.path     = item.value("path", "");

            if (!asset.id.empty()) {
                assets_.push_back(asset);
            }
        }
    }
    catch (...) {
        return false;
    }

    return true;
}

void AssetInfoLoader::Clear() {
    assets_.clear();
    definedFullIds.clear();
}

bool AssetInfoLoader::LoadGeneratedJson(const fs::path& jsonPath) {
    std::ifstream ifs(jsonPath);
    if (!ifs) return false;

    try {
        nlohmann::json j;
        ifs >> j;

        // JSONのルートキー（"Textures", "Models" など）をカテゴリとして扱う
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string category = it.key();

            // 再帰的にJSONを解析してアセットを抽出
            ParseRecursive(it.value(), category, {});
        }
    }
    catch (...) {
        return false;
    }
    return true;
}

void AssetInfoLoader::ParseRecursive(const nlohmann::json& j, const std::string& category, std::vector<std::string> hierarchy) {
    if (j.is_array()) {
        for (const auto& item : j) {
            if (item.is_string()) {
                std::string filePath = item.get<std::string>();
                fs::path p(filePath);
                std::string ext = p.extension().string();

                /// <summary>
                /// カテゴリごとに対象拡張子を制限
                /// </summary>
                if (category == "Models" && ext != ".obj") continue;
                if (category == "Textures" && ext != ".png") continue;
                if (category == "Audio" && ext != ".wav") continue;

                // 名前空間の重複を防ぐ
                std::vector<std::string> idHierarchy = hierarchy;
                if (!idHierarchy.empty() && idHierarchy[0] == category) {
                    idHierarchy.erase(idHierarchy.begin());
                }

                AssetDefinition asset;
                asset.category = category;
                asset.path = filePath; // パスはそのまま "Textures/..." を使う
                asset.subFolders = idHierarchy; // 名前空間用にはカテゴリを除いた階層を渡す
                asset.id = p.stem().string();

                // 重複チェック用キーも新しい階層で生成
                std::string uniqueKey = category;
                for (const auto& folder : idHierarchy) uniqueKey += "::" + folder;
                uniqueKey += "::" + asset.id;

                if (definedFullIds.contains(uniqueKey)) {
                    continue;
                }
                definedFullIds.insert(uniqueKey);
                assets_.push_back(asset);
            }
        }
    } else if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::vector<std::string> nextHierarchy = hierarchy;
            nextHierarchy.push_back(it.key());
            ParseRecursive(it.value(), category, nextHierarchy);
        }
    }
}