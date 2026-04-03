#include "Scanner.h"

using namespace AssetGen;

namespace fs = std::filesystem;

void Scanner::ResourceScan(const fs::path& rootDirectory, const std::function<void(const AssetEntry&)> onAssetFound) {
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(rootDirectory)) {
        AssetEntry currentAsset;

        // ルートからの相対パスを生成
        currentAsset.path = fs::relative(entry.path(), rootDirectory).wstring();

        // ファイル名（拡張子含む）s
        currentAsset.fileName = entry.path().filename().wstring();

        // ディレクトリかどうか
        currentAsset.isDirectory = entry.is_directory();

        // 拡張子（ファイルのみ意味あり）
        currentAsset.extension = entry.path().extension().wstring();

        // 呼び出し側へ通知
        onAssetFound(currentAsset);
    }
}

void Scanner::ScanAll(const fs::path& rootDirectory, std::vector<AssetEntry>& outEntries) {
    ResourceScan(rootDirectory, [&](const AssetEntry& asset) {
        outEntries.push_back(asset);
        });
}

void Scanner::ScanByExtension(const fs::path& rootDirectory, const std::wstring& extension, std::vector<AssetEntry>& outEntries) {
    ResourceScan(rootDirectory, [&](const AssetEntry& asset) {

        // ディレクトリは対象外
        if (asset.isDirectory) return;

        // 拡張子一致
        if (asset.extension == extension) {
            outEntries.push_back(asset);
        }
        });
}