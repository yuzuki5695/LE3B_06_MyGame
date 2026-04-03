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
        static std::set<std::string> definedFullIds;

        if (j.is_array()) {
            // 配列の場合：ファイルパスのリスト
            for (const std::string& filePath : j) {
                std::filesystem::path p(filePath);
                std::string ext = p.extension().string();

                /// <summary>
                /// カテゴリごとに対象拡張子を制限
                /// </summary>
                if (category == "Models" && ext != ".obj") continue;
                if (category == "Textures" && ext != ".png") continue;
                if (category == "Audio" && ext != ".wav") continue;

                AssetDefinition asset;
                asset.category = category;
                asset.path = filePath;
                asset.subFolders = hierarchy;          // フォルダ階層を保持
                asset.id = p.stem().string();          // ファイル名からID生成

                /// <summary>
                /// 重複チェック用キーを生成（カテゴリ + 階層 + ID）
                /// </summary>
                std::string uniqueKey = category;
                for (const auto& folder : hierarchy) uniqueKey += "::" + folder;
                uniqueKey += "::" + asset.id;

                if (definedFullIds.contains(uniqueKey)) {
                    std::cout << "Warning: Duplicate ID skipped -> " << uniqueKey << std::endl;
                    continue;
                }
                definedFullIds.insert(uniqueKey);

                assets_.push_back(asset);
            }
        } else if (j.is_object()) {
            // オブジェクトの場合：サブフォルダ
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::vector<std::string> nextHierarchy = hierarchy;

                // フォルダ名を階層に追加
                nextHierarchy.push_back(it.key());

                // 再帰的に探索
                ParseRecursive(it.value(), category, nextHierarchy);
            }
        }
    }