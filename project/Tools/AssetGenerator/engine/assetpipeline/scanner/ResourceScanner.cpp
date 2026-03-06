#include "ResourceScanner.h"
#include <AssetTypeDetector.h>
#include <PathNormalizer.h>
#include <filesystem>

namespace fs = std::filesystem;

ResourceScanner::ResourceScanner(const std::string& root) : root_(root) {}

std::vector<FileInfo> ResourceScanner::Scan() {
    std::vector<FileInfo> result;

    // ルートディレクトリから再帰的にファイルを走査
    for (const auto& entry : fs::recursive_directory_iterator(root_))
    {
        // ディレクトリはスキップし、ファイルのみ処理
        if (!entry.is_regular_file())
            continue;

        FileInfo info;

        // 相対パスを取得し、エンジンの規約に合わせて正規化
        std::string relative = fs::relative(entry.path(), root_).string();
        info.path = PathNormalizer::Normalize(relative);

        // ファイル名と拡張子の分離
        info.extension = entry.path().extension().string();
        info.name = entry.path().stem().string();

        // 拡張子からアセット種別を判定（対象外ならリストに入れない）
        info.type = AssetTypeDetector::Detect(info.extension);
        if (info.type == AssetType::Unknown)
            continue;

        // パスの最初の要素をグループ名（カテゴリ）として採用
        info.group = entry.path().begin()->string();

        result.push_back(info);
    }

    return result;
}