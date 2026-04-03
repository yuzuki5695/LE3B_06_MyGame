#include "AssetInfoLoader.h"
#include "AssetInfoLoader.h"
#include <fstream>
#include <json.hpp>
#include <iostream>

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

        // JSON構造例:
        // {
        //   "assets": [
        //     { "category": "Texture", "id": "Avoidance", "path": "Textures/ui/avoidance.png" },
        //     ...
        //   ]
        // }
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


void AssetInfoLoader::GenerateResourceIDHeader(const AssetInfoLoader& loader, const std::string& outputPath) {
    std::ofstream ofs(outputPath);
    if (!ofs) {
        std::cerr << "Failed to create header file: " << outputPath << std::endl;
        return;
    }

    // --- ヘッダの書き出し ---
    ofs << "#pragma once\n\n";
    ofs << "/// <summary>\n";
    ofs << "/// AssetGeneratorによって自動生成されたリソースID定義です。手動で編集しないでください。\n";
    ofs << "/// </summary>\n";
    ofs << "namespace LoadResourceID {\n";

    // カテゴリごとに整理して出力するために、一度mapに分類する
    std::map<std::string, std::vector<AssetDefinition>> categorizedAssets;
    for (const auto& asset : loader.GetAssets()) {
        categorizedAssets[asset.category].push_back(asset);
    }

    // カテゴリ（Texture, Modelなど）ごとにnamespaceを作成
    for (auto it = categorizedAssets.begin(); it != categorizedAssets.end(); ++it) {
        const std::string& category = it->first;
        const std::vector<AssetDefinition>& assets = it->second;

        ofs << "    namespace " << category << " {\n";

        for (const auto& asset : assets) {
            // 例: constexpr const char* Avoidance = "Textures/ui/avoidance.png";
            ofs << "        constexpr const char* " << asset.id << " = \"" << asset.path << "\";\n";
        }

        ofs << "    } // namespace " << category << "\n\n";
    }

    ofs << "} // namespace LoadResourceID\n";

    std::cout << "Generated: " << outputPath << std::endl;
}