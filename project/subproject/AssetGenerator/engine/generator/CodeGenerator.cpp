#include "CodeGenerator.h"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace AssetGen;

void CodeGenerator::GenerateResourceIDHeader(const std::vector<AssetDefinition>& assets, const std::filesystem::path& outputPath) {
    if (assets.empty()) return;

    // ツリー構築
    auto categoryTrees = BuildCategoryTrees(assets);

    // メモリ上（stringstream）にヘッダー内容をすべて書き出す
    std::stringstream ss;
    WriteHeader(ss, categoryTrees);
    std::string newContent = ss.str();

    // 既存のファイル内容と比較して、変更があるかチェック（高速化ロジック）
    bool shouldWrite = true;
    if (std::filesystem::exists(outputPath)) {
        std::ifstream ifs(outputPath, std::ios::binary);
        if (ifs) {
            // 既存ファイルの内容をすべて読み込む
            std::string currentContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            // 内容が完全に一致していれば書き込みをスキップ
            if (currentContent == newContent) {
                shouldWrite = false;
                std::cout << "AssetGenerator: " << outputPath.string() << " is up-to-date. (Skipped to speed up build)" << std::endl;
            }
        }
    }

    // 変更があった場合のみファイルへ書き出す
    if (shouldWrite) {
        // 出力先ディレクトリがない場合は作成
        if (outputPath.has_parent_path()) {
            std::filesystem::create_directories(outputPath.parent_path());
        }

        std::ofstream ofs(outputPath, std::ios::binary);
        if (!ofs) {
            std::cerr << "Failed to create header file: " << outputPath.string() << std::endl;
            return;
        }

        ofs << newContent;
        std::cout << "Generated: " << outputPath.string() << std::endl;
    }
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

void CodeGenerator::WriteNode(std::ostream& os, const Node& node, int indent) {
    std::string indentStr(indent * 4, ' ');

    // アセット出力
    for (const auto* asset : node.assets) {
        os << indentStr << "constexpr const char* " << asset->id << " = \"" << asset->path << "\";\n";
    }
    // 子namespace
    for (const auto& [name, child] : node.children) {
        os << indentStr << "namespace " << name << " {\n";
        WriteNode(os, child, indent + 1);
        os << indentStr << "} // namespace " << name << "\n";
    }
}

void CodeGenerator::WriteHeader(std::ostream& os, const std::map<std::string, Node>& categoryTrees) {
    os << "#pragma once\n\n";

    // 生成ファイル上部のコメントも英語化
    os << "/**\n"
        << " * @file LoadResourceID.h\n"
        << " * @brief Automatically generated resource identifiers. DO NOT EDIT DIRECTLY.\n"
        << " */\n\n";

    // 1層目: AssetGen
    os << "namespace AssetGen {\n";
    // 2層目: LoadResourceID
    os << "namespace LoadResourceID {\n\n";

    for (const auto& [category, node] : categoryTrees) {
        // 各カテゴリの前にコメントを挿入
        os << "    // ==========================================================\n";
        os << "    // Category: " << category << "\n";
        os << "    // ==========================================================\n";
        os << "    namespace " << category << " {\n";

        // インデントレベル開始
        WriteNode(os, node, 2);

        os << "    } // namespace " << category << "\n\n";
    }

    // 閉じ空間
    os << "} // namespace LoadResourceID\n";
    os << "} // namespace AssetGen\n";
}