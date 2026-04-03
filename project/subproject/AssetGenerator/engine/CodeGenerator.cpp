#include "CodeGenerator.h"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

void CodeGenerator::GenerateResourceIDHeader(const std::vector<AssetDefinition>& assets, const std::filesystem::path& outputPath) {
    if (assets.empty()) return;

    std::ofstream ofs(outputPath);
    if (!ofs) {
        std::cerr << "Failed to create header file: " << outputPath.string() << std::endl;
        return;
    }

    ofs << "#pragma once\n\n";
    ofs << "namespace LoadResourceID {\n";

    // カテゴリごとに整理
    std::map<std::string, std::vector<AssetDefinition>> categorized;
    for (const auto& asset : assets) {
        categorized[asset.category].push_back(asset);
    }

    for (const auto& [category, assetList] : categorized) {
        ofs << "    namespace " << category << " {\n";
        for (const auto& asset : assetList) {
            ofs << "        constexpr const char* " << asset.id << " = \"" << asset.path << "\";\n";
        }
        ofs << "    } // namespace " << category << "\n";
    }

    ofs << "} // namespace LoadResourceID\n";

    std::cout << "Generated: " << outputPath.string() << std::endl;
}