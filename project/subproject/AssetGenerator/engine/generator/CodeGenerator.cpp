#include "CodeGenerator.h"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace AssetGen;

void CodeGenerator::GenerateResourceIDHeader(const std::vector<AssetDefinition>& assets, const std::filesystem::path& outputPath) {
    if (assets.empty()) return;

    std::ofstream ofs(outputPath);
    if (!ofs) {
        std::cerr << "Failed to create header file: " << outputPath.string() << std::endl;
        return;
    }

    // 1. ツリー構築
    auto categoryTrees = BuildCategoryTrees(assets);

    // 2. 書き出し
    WriteHeader(ofs, categoryTrees);

    std::cout << "Generated: " << outputPath.string() << std::endl;
}

std::map<std::string, CodeGenerator::Node> CodeGenerator::BuildCategoryTrees(const std::vector<AssetDefinition>& assets) {
    std::map<std::string, Node> categoryTrees;

    for (const auto& asset : assets) {
        Node* node = &categoryTrees[asset.category];

        // 1階層目のみnamespace化
        if (!asset.subFolders.empty()) {
            node = &node->children[asset.subFolders[0]];
        }

        node->assets.push_back(&asset);
    }

    return categoryTrees;
}

void CodeGenerator::WriteNode(std::ofstream& ofs, const Node& node, int indent) {
    std::string indentStr(indent * 4, ' ');

    // アセット出力
    for (const auto* asset : node.assets) {
        ofs << indentStr
            << "constexpr const char* " << asset->id
            << " = \"" << asset->path << "\";\n";
    }

    // 子namespace
    for (const auto& [name, child] : node.children) {
        ofs << indentStr << "namespace " << name << " {\n";

        WriteNode(ofs, child, indent + 1);

        ofs << indentStr << "} // namespace " << name << "\n";
    }
}

void CodeGenerator::WriteHeader(std::ofstream& ofs, const std::map<std::string, Node>& categoryTrees) {
    ofs << "#pragma once\n\n";
    // 出力されるヘッダーも AssetGen 名前空間で囲む 
    ofs << "namespace AssetGen {\n";
    ofs << "namespace LoadResourceID {\n";

    for (const auto& [category, tree] : categoryTrees) {
        ofs << "    namespace " << category << " {\n";

        WriteNode(ofs, tree, 2);

        ofs << "    } // namespace " << category << "\n";
    }

    ofs << "} // namespace LoadResourceID\n";
    ofs << "} // namespace AssetGen\n";
}