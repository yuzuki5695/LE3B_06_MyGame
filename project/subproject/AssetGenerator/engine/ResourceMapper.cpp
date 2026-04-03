#include "ResourceMapper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>

namespace fs = std::filesystem;

void ResourceMapper::UpdateSingle(const fs::path& targetDir, const std::vector<std::wstring>& extensions, const fs::path& outputPath) {
    Scanner scanner;
    std::vector<AssetEntry> assets;

    std::unordered_set<std::wstring> extSet(extensions.begin(), extensions.end());

    scanner.ResourceScan(targetDir, [&](const AssetEntry& asset) {

        if (asset.isDirectory) return;

        if (extSet.contains(asset.extension)) {
            assets.push_back(asset);
        }
        });

    // --- ツリー構築 ---
    ResourceNode rootNode;
    for (const auto& asset : assets) {
        fs::path p(asset.path);

        std::vector<std::wstring> parts;
        for (const auto& part : p) {
            parts.push_back(part.wstring());
        }

        rootNode.addFile(parts, asset.path);
    }

    // --- JSON ---
    nlohmann::json j;
    j["resources"] = rootNode.serialize();

    SaveIfChanged(j.dump(4), outputPath);
}

// ========================================
// コンストラクタ
// ========================================

/// <summary>
/// 出力先とスキャン対象ルートを初期化する
/// </summary>
ResourceMapper::ResourceMapper(const fs::path& root, const fs::path& output) : resourceRoot(root), outputPath(output) {}

void ResourceMapper::UpdateTextureList() {
    //Scanner scanner;
    //std::vector<AssetEntry> pngAssets;
    //scanner.ScanByExtension(resourceRoot, L".png", pngAssets);

    //ResourceNode rootNode;
    //for (const auto& asset : pngAssets) {
    //    fs::path p(asset.path);
    //    std::vector<std::wstring> parts;
    //    for (const auto& part : p) { parts.push_back(part.wstring()); }
    //    rootNode.addFile(parts, asset.path);
    //}

    //// --- JSON生成部分 ---
    //nlohmann::json finalJson;
    //finalJson["textures"] = rootNode.serialize();

    //// dump(4) でスペース4つのインデント付き文字列に変換
    //SaveIfChanged(finalJson.dump(4));
}

// ========================================
// 差分保存
// ========================================
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

// ========================================
// ファイル読み込み
// ========================================

/// <summary>
/// 指定ファイルをすべて読み込んで文字列として返す
/// </summary>
std::string ResourceMapper::ReadAllText(const fs::path& path) {
    std::ifstream ifs(path, std::ios::binary);

    if (!ifs) return "";

    std::stringstream buffer;
    buffer << ifs.rdbuf();

    return buffer.str();
}

