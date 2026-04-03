#include "ResourceMapper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>

using namespace AssetGen;

namespace fs = std::filesystem;

ResourceMapper::ResourceMapper(const fs::path& resourceRoot, const fs::path& defaultOutputPath) : resourceRoot(resourceRoot), outputPath(defaultOutputPath) {}

void ResourceMapper::UpdateSingle(const fs::path& targetDirectory, const std::vector<std::wstring>& extensions, const fs::path& outputPath) {
    // ========================================
    // ① スキャン
    // ========================================
    Scanner scanner;
    std::vector<AssetEntry> assets;

    // 拡張子検索を高速化（O(1)）
    std::unordered_set<std::wstring> extensionSet(
        extensions.begin(), extensions.end());

    scanner.ResourceScan(targetDirectory, [&](const AssetEntry& asset) {

        // ディレクトリ除外
        if (asset.isDirectory) return;

        // 拡張子フィルタ
        if (extensionSet.contains(asset.extension)) {
            assets.push_back(asset);
        }
        });

    // ========================================
    // ② ツリー構築
    // ========================================
    ResourceNode rootNode;

    for (const AssetEntry& asset : assets) {

        fs::path path(asset.path);

        // パス分割
        std::vector<std::wstring> parts;
        for (const fs::path& part : path) {
            parts.push_back(part.wstring());
        }

        // ツリーに登録
        rootNode.AddFile(parts, asset.path);
    }

    // ========================================
    // ③ JSON生成
    // ========================================
    nlohmann::json json;
    std::string key = targetDirectory.filename().string();
    json[key] = rootNode.serialize();

    // ========================================
    // ④ 保存（差分チェック付き）
    // ========================================
    SaveIfChanged(json.dump(4), outputPath);
}

void ResourceMapper::SaveIfChanged(const std::string& content, const std::filesystem::path& path) {
    // --- 既存ファイル読み込み ---
    std::ifstream ifs(path);
    if (ifs) {
        std::stringstream buffer;
        buffer << ifs.rdbuf();

        // --- 内容が同じなら何もしない ---
        if (buffer.str() == content) {
            std::cout << "変更なし: " << path.string() << std::endl;
            return;
        }
    }

    // --- 書き込み ---
    std::ofstream ofs(path);
    ofs << content;

    std::cout << "更新: " << path.string() << std::endl;
}

std::string ResourceMapper::ReadAllText(const fs::path& path) {
    std::ifstream ifs(path, std::ios::binary);

    if (!ifs) return "";

    std::stringstream buffer;
    buffer << ifs.rdbuf();

    return buffer.str();
}

void ResourceMapper::UpdateByRules(const std::vector<ExportRule>& rules) {
    for (const auto& rule : rules) {
        UpdateSingle(rule.dir, rule.extensions, rule.output);
    }
}